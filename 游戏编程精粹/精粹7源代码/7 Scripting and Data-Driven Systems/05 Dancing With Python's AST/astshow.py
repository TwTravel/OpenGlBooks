# author: Zou Guangxian
# date : 2005-3-2
import sys
import types
import parser , symbol, token
import pprint

def dump( node ,output,indent ):
    name = symbol.sym_name.get( node[0] )
    if name is None:
        name = token.tok_name.get( node[0] )
    output.append( [ name ] )
    for i in range( 1, len(node) ):
        item = node[i]
        if  type(item) is types.ListType:
            dump( item , output[-1], indent + 2 )
        else:
            output[-1].append( repr(item) )

def parse( source ):
    ast = parser.suite( source )
    tup = ast.tolist()
    ident = 0
    output = []
    dump( tup , output,0 )    
    pprint.pprint( output )


if __name__=='__main__':
    if ( len(sys.argv) != 2 ):
        print 'usage: %s %s ' % ( sys.argv[0], "sample.py" )
        sys.exit()
    parse( open(sys.argv[1]).read() )
