# -*- coding: GBK -*-
# $Id: main.py 20 2005-12-31 03:34:33Z zouguangxian $
import os
import sys
import getopt
import preprocess
import postprocess
import utils
import compiler

class processor:
    def __init__( self, target, action, resfile, functions ):
        self.target = target
        self.action = action
        self.handler = getattr(self, 'do_%s'%(action), None)
        self.visitor = preprocess.Visitor()
        utils.helper.reset()
        utils.helper.open( resfile )
        utils.helper.functions = functions
        
    def process( self ):
        self.dispatch( self.target )
        utils.helper.close()
        
    def process_file( self, target ):
        if target[-3:] != '.py':
            return        
        print '[p] %s' % ( target )
        self.handler( target )
        
    def process_dir( self, target ):
        for root, dirs, files in os.walk(target):
            for name in files:
                self.process_file( os.path.join( root, name ) )
                
            if 'CVS' in dirs:
                dirs.remove('CVS')
            if '.svn' in dirs:
                dirs.remove('.svn')

    def dispatch( self, target ):
        if os.path.isfile( target ):
            self.process_file( target )
        elif os.path.isdir( target ):
            self.process_dir( target )
                    
    def do_scan( self, filename ):
        tree = compiler.parseFile( filename )    
        try:
            compiler.walk( tree, self.visitor ) 
        except SyntaxError,err:
            print '[e] %s' % ( err )
                    
    def do_compile( self, filename ):
        saved = compiler.pycodegen.CodeGenerator.visitCallFunc
        compiler.pycodegen.CodeGenerator.visitCallFunc = postprocess.visitCallFunc
        try:
            compiler.pycodegen.compileFile( filename )
        except SyntaxError,err:
            print '[e] %s' % ( err )
        compiler.pycodegen.CodeGenerator.visitCallFunc = saved
                
    def do_all( self, filename ):
        self.do_scan( filename )
        self.do_compile( filename )
        
def usage():
    print 'usage: python %s [--scan|--compile|--all] [-s stringres.txt] dest' % sys.argv[0]
    print '--scan    : extract the string'
    print '--compile : compile the source code'
    print '--all     : scan first and then compile'
    print 'dest    : python file or directory contain python file'
    
def main():
    try:
        opts, args = getopt.getopt( sys.argv[1:], 's:', 
                ['scan', 'compile', 'all'] )
    except getopt.error, msg:
        print msg
        usage()

    resfile = None
    action = None
    for o, a in opts:
        if o == '-s': resfile = a
        if o in ['--scan','--compile','--all']:
            action = o[2:]

    if len(args) != 1:
        usage()
        sys.exit(2)
    functions = [ 'game.Talk' ]
    handler = processor( args[0], action, resfile, functions )
    handler.process()
    
if __name__ == '__main__':
    main()
