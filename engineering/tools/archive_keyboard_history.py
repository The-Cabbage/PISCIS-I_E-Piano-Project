"""Read Git objects into ordinary ZIP snapshots; never modify source repositories."""
import csv
import hashlib
import json
from pathlib import Path
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'engineering/archives/keyboard-history-2026-10-23'
MODULES = ['Keyboard', 'Keyboard_Scanning', 'Keyboard_Scanning_ScaleUp',
           'Keyboard_Scanning_ScaleUp_PogoPin']


def run(repo, *args):
    return subprocess.run(['git', '-C', str(repo), *args], capture_output=True)


def export(repo, dest):
    dest.mkdir(parents=True, exist_ok=False)
    inventory = run(repo, 'cat-file', '--batch-all-objects',
                    '--batch-check=%(objectname) %(objecttype) %(objectsize)')
    if inventory.returncode:
        raise RuntimeError(inventory.stderr.decode(errors='replace'))
    commits = [line.split()[0].decode() for line in inventory.stdout.splitlines()
               if line.split()[1] == b'commit']
    proc = subprocess.Popen(['git', '-C', str(repo), 'cat-file', '--batch'],
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    cache = {}
    cache_bytes = 0

    def obj(oid):
        nonlocal cache_bytes
        if oid in cache:
            return cache[oid]
        proc.stdin.write(oid.encode() + b'\n')
        proc.stdin.flush()
        header = proc.stdout.readline().split()
        if len(header) != 3:
            raise RuntimeError('Unavailable object: ' + oid)
        data = proc.stdout.read(int(header[2]))
        assert proc.stdout.read(1) == b'\n'
        typ = header[1]
        actual = hashlib.sha1(typ + b' ' + str(len(data)).encode() + b'\0' + data).hexdigest()
        if actual != oid:
            raise RuntimeError('Object hash mismatch: ' + oid)
        if cache_bytes > 256 * 1024 * 1024:
            cache.clear()
            cache_bytes = 0
        cache[oid] = data
        cache_bytes += len(data)
        return data

    fsck = run(repo, 'fsck', '--full')
    (dest / 'source-fsck.txt').write_bytes(fsck.stdout + fsck.stderr)
    refs = run(repo, 'show-ref', '--head')
    (dest / 'source-refs.txt').write_bytes(refs.stdout + refs.stderr)
    records, errors, checksums = [], [], []
    z = None
    part = 0
    manifest = []

    def close_part():
        nonlocal z, manifest
        if z is None:
            return
        z.writestr('SHA256.json', json.dumps(manifest, ensure_ascii=False, indent=2))
        filename = Path(z.filename)
        z.close()
        with zipfile.ZipFile(filename) as check:
            assert len(check.infolist()) == len(manifest) + 1
            for entry in manifest:
                data = check.read(entry['path'])
                assert hashlib.sha256(data).hexdigest() == entry['sha256']
        checksums.append({'file': filename.name,
                          'sha256': hashlib.sha256(filename.read_bytes()).hexdigest(),
                          'bytes': filename.stat().st_size,
                          'verified_files': len(manifest)})
        z, manifest = None, []

    try:
        for number, oid in enumerate(commits, 1):
            try:
                raw = obj(oid)
                headers, _, msg = raw.partition(b'\n\n')
                tree = next(x[5:].decode() for x in headers.splitlines() if x.startswith(b'tree '))
                committer = next(x[10:].decode(errors='replace') for x in headers.splitlines()
                                 if x.startswith(b'committer '))
                listing = run(repo, 'ls-tree', '-rz', '--full-tree', tree)
                if listing.returncode:
                    raise RuntimeError(listing.stderr.decode(errors='replace'))
                files = []
                for entry in listing.stdout.split(b'\0'):
                    if not entry:
                        continue
                    meta, path = entry.split(b'\t', 1)
                    mode, typ, blob = meta.split()
                    if typ != b'blob':
                        raise RuntimeError('Unsupported nested gitlink: ' + repr(path))
                    files.append((path.decode('utf-8'), mode.decode(), blob.decode()))
                # Check all content availability before writing this snapshot.
                for _, _, blob in files:
                    obj(blob)
                if z is None:
                    part += 1
                    z = zipfile.ZipFile(dest / f'snapshots-{part:03}.zip', 'x',
                                        zipfile.ZIP_DEFLATED, compresslevel=3)
                archive = Path(z.filename).name
                for path, mode, blob in files:
                    name = 'snapshots/' + oid + '/' + path
                    if path.startswith('/') or '..' in Path(path).parts:
                        raise RuntimeError('Unsafe snapshot path: ' + path)
                    data = obj(blob)
                    info = zipfile.ZipInfo(name)
                    info.compress_type = zipfile.ZIP_DEFLATED
                    info.external_attr = int(mode, 8) << 16
                    z.writestr(info, data, compresslevel=3)
                    manifest.append({'path': name, 'sha256': hashlib.sha256(data).hexdigest(),
                                     'git_blob': blob, 'mode': mode})
                records.append({'commit': oid, 'committer': committer,
                                'message': msg.decode(errors='replace').strip(),
                                'parents': ' '.join(x[7:].decode() for x in headers.splitlines()
                                                    if x.startswith(b'parent ')),
                                'archive': archive, 'folder': 'snapshots/' + oid,
                                'files': len(files)})
                if z.fp.tell() >= 64 * 1024 * 1024:
                    close_part()
            except (RuntimeError, StopIteration) as exc:
                errors.append({'commit': oid, 'error': str(exc)})
            if number % 250 == 0:
                print(f'{repo.relative_to(ROOT)}: {number}/{len(commits)}', flush=True)
        close_part()
    finally:
        proc.stdin.close()
        proc.wait()
    with (dest / 'index.csv').open('w', encoding='utf-8-sig', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=['commit', 'committer', 'message', 'parents',
                                              'archive', 'folder', 'files'])
        writer.writeheader()
        writer.writerows(records)
    summary = {'source': str(repo.relative_to(ROOT)), 'source_head':
               run(repo, 'rev-parse', 'HEAD').stdout.decode().strip(),
               'discoverable_commit_objects': len(commits), 'exported_commits': len(records),
               'export_errors': errors, 'source_fsck_exit': fsck.returncode,
               'archives': checksums, 'verification': 'All exported bytes matched Git object SHA-1; '
               'all snapshot ZIP members re-read and matched SHA-256. No KiCad GUI restore test.'}
    (dest / 'summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2))
    print(json.dumps({k: v for k, v in summary.items() if k != 'archives'}, ensure_ascii=False), flush=True)
    return summary


def export_supplements():
    """Preserve loose backup files and untracked history notes outside commit snapshots."""
    target = OUT / 'loose-backups.zip'
    manifest = []
    with zipfile.ZipFile(target, 'x', zipfile.ZIP_DEFLATED, compresslevel=3) as z:
        for module in MODULES:
            for path in sorted((ROOT / module).rglob('*')):
                if not path.is_file() or path.is_symlink() or '.git' in path.parts:
                    continue
                rel = path.relative_to(ROOT)
                if path.suffix not in {'.bak', '.bck', '.kicad_sch-bak', '.kicad_pcb-bak'} and not (
                        '.history' in rel.parts and path.name in {'README.txt', '.gitignore'}):
                    continue
                data = path.read_bytes()
                z.writestr(str(rel), data)
                manifest.append({'path': str(rel), 'sha256': hashlib.sha256(data).hexdigest()})
        z.writestr('SHA256.json', json.dumps(manifest, ensure_ascii=False, indent=2))
    with zipfile.ZipFile(target) as z:
        for item in manifest:
            assert hashlib.sha256(z.read(item['path'])).hexdigest() == item['sha256']
    (OUT / 'loose-backups-summary.json').write_text(json.dumps({
        'file': target.name, 'sha256': hashlib.sha256(target.read_bytes()).hexdigest(),
        'verified_files': len(manifest), 'entries': manifest}, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    summaries = []
    for module in MODULES:
        for kind, repo in [('project', ROOT / module), ('kicad-local-history', ROOT / module / '.history')]:
            summaries.append(export(repo, OUT / module / kind))
    (OUT / 'summary.json').write_text(json.dumps(summaries, ensure_ascii=False, indent=2))
    export_supplements()
