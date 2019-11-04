# -*- coding: GBK -*-
# $Id: utils.py 20 2005-12-31 03:34:33Z zouguangxian $
import os
import sys
from compiler import ast

def getFullName( node ):
    'return the fullname of a node'
    if isinstance(node, ast.Name):
        return node.name
    elif isinstance(node, ast.Getattr):
        return getFullName(node.getChildNodes()[0]) + '.' + node.attrname
    else:
        return ''

class Helper:
    def __init__( self ):       
        self.reset()
    
    def nextId( self ):
        self.curId = self.curId + 1
        return self.curId

    def open( self, filename ):
        'read relationship from file'
        self.reset()
        self.filename = filename

        if not os.path.exists( filename ):
            return

        file = open( filename, 'r' )
        
        for line in file.readlines():
            line = line.strip('\r\n')
            row = line.split('\t')
            row.extend( [''] * ( 2 - len(row) ) ) 

            if self.map.has_key( row[1] ):
                continue

            id = int( row[0] )
            if id > self.curId :
                self.curId = id

            self.map[ row[1] ] = id
            
        file.close()

    def reset( self ):
        'reset internal state'
        self.curId = 0
        self.map={}
        self.functions={}
        self.filename = None

    def close( self ):
        if self.filename == None:
             return
        self.write( self.filename )

    def write( self, filename ):
        'write the relationship to file'
        file = open( filename, 'w' )

        result = {}
        for key, value in self.map.items():
            result.update( {value:key} )
        result.items().sort()

        for key, value in result.items():
            file.write( '%d\t%s\n' % ( key, value ) )

        file.close()

    def append( self, value ):
        'maintain the relationship between string and id'
        if ( self.map.has_key( value ) ):
            return
        self.map[ value ] = self.nextId()

    def get( self, value ):
        'return the id if the string exist, otherwise return itself'
        if ( self.map.has_key( value ) ):
            return self.map[ value ]
        return value

            
helper = Helper()

