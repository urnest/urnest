#!/bin/env python3

import sys

def main(argv):
    ODIN_classpath=argv[1]
    if len(argv)==3:
        out=open(argv[2], 'w')
    else:
        out=sys.stdout
        pass
    
    classpath=[]
    if ODIN_classpath:
        classpath=open(ODIN_classpath).read().splitlines()
        pass
    single_jars=[x for x in classpath if \
                 x.endswith('.jar') or \
                 x.endswith('*.JAR')]
    for j in single_jars:
        print("%(j)s" % vars(),file=out)
        pass

    wild_jars=[x for x in classpath if not (x.endswith('.jar') or \
                 x.endswith('*.JAR')) ]
    for j in wild_jars:
        print("%(j)s:dir.jarlist.tree" % vars(),file=out)
        pass
    pass

    pass

if __name__=='__main__':
    try:
        main(sys.argv)
    except:
        sys.stderr.write(str(sys.exc_info()[1])+'\n')
        sys.exit(1)
    pass
