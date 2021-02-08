#!/usr/bin/env python

def split(s):
    def repl(s):
        ss = s.replace("  ", " ")
        return repl(ss) if ss != s else s

    return repl(s.strip().replace("\t", " ")).split(" ")

print(split("             jan           de        visser"))