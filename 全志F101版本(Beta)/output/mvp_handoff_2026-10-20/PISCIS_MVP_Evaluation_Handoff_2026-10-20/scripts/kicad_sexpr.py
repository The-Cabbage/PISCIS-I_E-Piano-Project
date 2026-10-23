"""Small read-only S-expression reader for KiCad geometry verification."""
import re,json,math
from pathlib import Path

def parse(path):
    tokens = iter(re.findall(r'"(?:\\.|[^"\\])*"|[^\s()]+|[()]',Path(path).read_text()))
    def one(t):
        if t=='(':
            a=[]
            for t in tokens:
                if t==')': return a
                a.append(one(t))
            raise ValueError('unclosed')
        return json.loads(t) if t.startswith('"') else t
    return one(next(tokens))
def children(a,key): return [v for v in a if isinstance(v,list) and v and v[0]==key]
def child(a,key): return next(iter(children(a,key)),None)
def prop(a,key): return next((v[2] for v in children(a,'property') if v[1]==key),'')
