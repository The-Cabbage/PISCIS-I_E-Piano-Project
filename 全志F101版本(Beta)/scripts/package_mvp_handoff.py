#!/usr/bin/env python3
"""Create or verify a portable F101 project snapshot. Never runs KiCad generators."""
import argparse
import hashlib
import json
import re
import subprocess
import sys
import zipfile
from datetime import datetime, timedelta, timezone
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
SKIP_PARTS = {'.git', '.history', '__pycache__', 'output', 'node_modules'}
SKIP_NAMES = {'.DS_Store'}
SKIP_SUFFIXES = {'.lck', '.kicad_prl', '.bak', '.pyc'}
MANIFEST = 'MANIFEST.json'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def source_files():
    for path in sorted(ROOT.rglob('*')):
        rel = path.relative_to(ROOT)
        if any(p in SKIP_PARTS for p in rel.parts):
            continue
        if path.name in SKIP_NAMES or path.suffix in SKIP_SUFFIXES:
            continue
        if path.is_symlink():
            raise ValueError('Unexpected symlink: ' + str(rel))
        if path.is_file() and rel.as_posix() != MANIFEST:
            yield path, rel


def local_links(root):
    """Check Markdown link destinations; raw upstream context .txt is not navigation."""
    errors = []
    for path in root.rglob('*.md'):
        body = re.sub(r'```.*?```', '', path.read_text(encoding='utf-8'), flags=re.S)
        for target in re.findall(r'\]\((<[^>]+>|[^\s]+?)(?:\s+"[^"]*")?\)', body):
            target = target.strip('<>')
            parsed = urlsplit(target)
            if parsed.scheme or parsed.netloc:
                continue
            dest = (path.parent / unquote(parsed.path)).resolve() if parsed.path else path.resolve()
            if not dest.is_relative_to(root.resolve()):
                errors.append(f'{path.relative_to(root)}: link leaves package: {target}')
            elif not dest.exists():
                errors.append(f'{path.relative_to(root)}: missing target: {target}')
            elif parsed.fragment and dest.suffix == '.md':
                text = dest.read_text(encoding='utf-8')
                anchors = set(re.findall(r'<a\s+id=["\']([^"\']+)["\']', text))
                for heading in re.findall(r'^#{1,6}\s+(.+?)\s*#*$', text, re.M):
                    heading = re.sub(r'[^\w\-\s]', '', heading.lower()).replace(' ', '-')
                    anchors.add(heading)
                if unquote(parsed.fragment) not in anchors:
                    errors.append(f'{path.relative_to(root)}: missing anchor: {target}')
    return errors


def verify(root):
    root = root.resolve()
    manifest = json.loads((root / MANIFEST).read_text(encoding='utf-8'))
    expected, errors = set(), []
    for row in manifest['files']:
        rel = row['path']
        path = (root / rel).resolve()
        if not path.is_relative_to(root) or Path(rel).is_absolute() or rel in expected:
            errors.append('Invalid/duplicate manifest path: ' + rel)
            continue
        expected.add(rel)
        if not path.is_file():
            errors.append('Missing: ' + rel)
            continue
        data = path.read_bytes()
        if sha(data) != row['sha256'] or len(data) != row['size_bytes']:
            errors.append('Content changed: ' + rel)
    actual = {p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file()}
    errors.extend('Extra file: ' + r for r in sorted(actual - expected - {MANIFEST}))
    errors.extend(local_links(root))
    if errors:
        raise ValueError('\n'.join(errors))
    return len(expected)


def git_output(args):
    result = subprocess.run(['git'] + args, cwd=ROOT, capture_output=True, text=True)
    return result.stdout.strip() if result.returncode == 0 else None


def build(output):
    output = output.resolve()
    if not output.is_relative_to(ROOT/'output'):
        raise ValueError('Use a new output directory under this project\'s output/ directory.')
    if output.exists():
        raise ValueError('Refusing to overwrite existing output directory: ' + str(output))
    required = ['AGENTS.md', 'README.md', 'DECISIONS.md',
                'docs/mvp_handoff/README.md', 'docs/mvp_handoff/DATASHEETS.md',
                'docs/mvp_handoff/reports/file_audit_2026-10-20.json',
                'docs/mvp_handoff/datasheets/SOURCES.json']
    for rel in required:
        if not (ROOT/rel).is_file():
            raise ValueError('Missing required handoff file: ' + rel)
    captured = datetime.now(timezone(timedelta(hours=8)))
    folder_name = 'PISCIS_MVP_Evaluation_Handoff_' + captured.strftime('%Y-%m-%d')
    staged = output / folder_name
    staged.mkdir(parents=True)
    records = []
    for path, rel in source_files():
        data = path.read_bytes()
        dest = staged / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(data)
        records.append(dict(path=rel.as_posix(), size_bytes=len(data), sha256=sha(data)))
    manifest = dict(
        format_version=1, captured_at=captured.isoformat(), timezone='Asia/Shanghai',
        scope='Complete current F101 subproject files plus explicit upstream context text snapshots; no Git history or other parent projects.',
        entrypoint='docs/mvp_handoff/README.md',
        repository_context=dict(parent_head=git_output(['rev-parse','HEAD']),
                                project_status=git_output(['status','--short','--','.']),
                                note='Parent HEAD does not identify this mostly untracked project snapshot.'),
        exclusions=dict(directory_names=sorted(SKIP_PARTS),filenames=sorted(SKIP_NAMES),suffixes=sorted(SKIP_SUFFIXES)),
        checks=dict(hashes=True,local_markdown_links=True,design_regeneration=False,
                    erc_drc_rerun=False,physical_verification=False),
        files=records)
    (staged / MANIFEST).write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    count = verify(staged)
    for row in records:
        if sha((ROOT/row['path']).read_bytes()) != row['sha256']:
            raise ValueError('Source changed during packaging: ' + row['path'])
    archive = output / (folder_name + '.zip')
    with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as zf:
        for path in sorted(staged.rglob('*')):
            if path.is_file():
                zf.write(path, folder_name + '/' + path.relative_to(staged).as_posix())
    with zipfile.ZipFile(archive) as zf:
        bad = zf.testzip()
        if bad:
            raise ValueError('ZIP CRC failed: ' + bad)
        if len(zf.namelist()) != count + 1:
            raise ValueError('ZIP entry count mismatch')
    digest = sha(archive.read_bytes())
    (output/'ZIP_SHA256.txt').write_text(digest + '  ' + archive.name + '\n',encoding='utf-8')
    print(json.dumps(dict(package_directory=str(staged),zip=str(archive),
                         files=count,zip_bytes=archive.stat().st_size,zip_sha256=digest,
                         checks='manifest hashes, local Markdown links, source stability, ZIP CRC'),
                     ensure_ascii=False,indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--output-dir',type=Path)
    mode.add_argument('--verify',type=Path)
    args = parser.parse_args()
    if args.verify:
        count = verify(args.verify)
        print(f'PASS: {count} file hashes and local Markdown links verified.')
    else:
        build(args.output_dir or ROOT/'output/mvp_handoff_2026-10-20')


if __name__ == '__main__':
    try:
        main()
    except (ValueError,OSError,KeyError) as error:
        print('ERROR: ' + str(error),file=sys.stderr)
        sys.exit(1)
