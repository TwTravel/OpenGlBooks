#!/usr/pkg/awk
#
# Copyright 2004, the OpenGLUT contributors.
#
# THIS IS AN UGLY HACK.
#
# But Doxygen doesn't produce real man-pages, and I want real man-
# pages.  (^&
#
# Usage (note that some path-name choices are hard-coded):
#
#  cd to a suitable spot in your filesystem.
#  awk -f path1/dox.awk <path2/file.c
#  cd man
#  ls *.3 | sed ... | sh
#
# path1 is wherever you keep th is script
# path2 is wherever a Doxygenated sourcefile is lying about.
#
# The *roff macro package used is -mdoc, from the groff(1) package.
# Both GNU's gawk and the nawk package used on NetBSD have been
# used to test this script.  It is hoped that it stands a chance
# of working on other AWKs.  Please report any problems---and if
# you have trouble, try using gawk.
#
# This is a simple AWK program to extract the restricted, stylized
# Doxygen markup that we are using in OpenGLUT.  The goal is to produce
# usable man-pages, since the best that Doxygen seems capable of doing
# is to emit a useless mass of prose-with-markup for each "module".
# But man-pages are per-function quick references, not per-module
# essays, as a rule.  Doxygen's closest thing to a concession to this
# basic fact is support for writing multiple copies of the per-module
# essays, one full copy per function.  Other than relieving you of
# the duty of knowing the right module for a function, that is not
# very useful.
#
# The heuristic that we use here is to look for the /*! to kick us
# into Doxygen-block-comments.  If the first line after that matches
#
#   [ ]*\fn
#
# ...then we assume that we are marking up a function and collect
# everything down to the */ line followed by the function head (ANSI style)
# and then the open-brace.  We then re-write the resulting data into
# a .3 *roff source file.
#
# We separate \brief, \note, \bug, and so forth, as we go along.
# When we get to the open-brace, we do some further processing as
# we spit out the collected information.
#
# This is a bit cheesey, but it is much better in terms of utility
# than the Doxygen man-pages.
#
# A word on style: EMACS has an AWK mode, but it is a little too agressive
# (and often results in illegal programs).  Part of my defense against
# EMACS on this front is to just insert optional {braces} everywhere.
# I don't care for that at all (it's cluttered) but that's better than
# fixing up the indentation everytime I add or remove a {brace}.
#

# This function (re)initializes our state variables.
#
function init() {
    path_to_dump_to = "man/";
    state = "init";
    brief = "";
    group = "";
    func_name = "";
    func_type = "";
    authors = 0;
    bugs    = 0;
    docs    = 0;
    lines   = 0;
    notes   = 0;
    params  = 0;
    sees    = 0;
    todos   = 0;
    line_state = "unhandled";
}

# This function is intended to strip off prefixes such as
# \see, \note, \bug, \todo, ...
#
function tail(s) {
    ret = s;
    sub( "^[ ]*\\\\[a-zA-Z]*[ ]*", "", ret );
    return ret;
}

function doxy_func_mode( state ) {
    return ( "doxy-func" == state ) ||
        ( "doxy-func-author" == state ) ||
        ( "doxy-func-brief"  == state ) ||
        ( "doxy-func-param"  == state ) ||
        ( "doxy-func-see"    == state ) ||
        ( "doxy-func-note"   == state ) ||
        ( "doxy-func-bug"    == state ) ||
        ( "doxy-func-todo"   == state );
}


BEGIN{
    init( );
}

# Match /*! lines (an ugly regular expression, eh?).
#
/^\/\*\!/{
    state = "doxy-block";
    line_state = "handled";
}

# Matches \file lines.  Just announces the filename and otherwise
# ignores it.
#
/^[ ]*\\file/{
    print "";
    print "---";
    print " " tail( $0 );
    line_state = "handled";
}

# When we find a \fn, we assume that we
# are entering a function's documentation block.
#
/^[ ]*\\fn/{
    if( "doxy-block" == state )
    {
        state = "doxy-func";
    }
    line_state = "handled";
}

# A function should have only one \brief, though
# it may span more than one line.
#
/^[ ]*\\brief/{
    if( doxy_func_mode( state ) )
    {
        brief = tail( $0 );
        state = "doxy-func-brief";
    }
    line_state = "handled";
}

# Presently we don't do anything with \ingroup.
#
/^[ ]*\\ingroup/{
    if( doxy_func_mode( state ) )
    {
        group = tail( $0 );
    }
    line_state = "handled";
}

# We presently throw away the "param" entries, but
# we might use them later.  (We should allow them to
# span lines, too, I guess...)
#
/^[ ]*\\param/{
    if( doxy_func_mode( state ) )
    {
        param[params] = tail( $0 );
        state = "doxy-func-param";
        ++params;
    }
    line_state = "handled";
}

# "SEE ALSO" cross references.  I really should allow
# these to span lines...
#
/^[ ]*\\see/{
    if( doxy_func_mode( state ) )
    {
        see[sees] = tail( $0 );
        state = "doxy-func-see";
        ++sees;
    }
    line_state = "handled";
}

# \notes can span lines.  They are transformed into
# a CAVEATS section near the bottom of the man-page.
#
/^[ ]*\\note/{
    if( doxy_func_mode( state ) )
    {
        note[notes] = tail( $0 );
        ++notes;
        state = "doxy-func-note";
    }
    line_state = "handled";
}

# Transformed like \note, but to a BUGS section
# rather than CAVEATS.
#
/^[ ]*\\bug/{
    if( doxy_func_mode( state ) )
    {
        bug[bugs] = tail( $0 );
        ++bugs;
        state = "doxy-func-bug";
    }
    line_state = "handled";
}

/^[ ]*\\todo/{
    if( doxy_func_mode( state ) )
    {
        todo[todos] = tail( $0 );
        ++todos;
        state = "doxy-func-todo";
    }
    line_state = "handled";
}

/^[ ]*\\author/{
    if( doxy_func_mode( state ) )
    {
        author[authors] = tail( $0 );
        ++authors;
        state = "doxy-func-author";
    }
    line_state = "handled";
}

/^[ ]*\\internal/{
    if( doxy_func_mode( state ) )
    {
    }
    line_state = "handled";
}

/^[ ]*\\[a-zA-Z0-9]/{
    if( ( "unhandled" == line_state ) &&
        ( "\\a"       != $1 ) )
    {
        print "Error (line " NR "): Got unhandled symbol " $1;
        print;
        print "Could be an unknown symbol, or could be that it ";
        print "occurred in a context where I don't know what to do.";
        print "";
        line_state = "handled";
    }
}

# The end-of-comment, */, indicates that we are
# done with a Doxygen documentation block, if we
# were in one.  Now we scan for for a function name.
#
/^\*\//{
    if( doxy_func_mode( state ) )
    {
        state = "doxy-func-name";
        line_state = "handled";
    }
}

# This function strips off everything up to and including
# "OGAPIENTRY", and also everything after and including
# any parentheses that follow OGAPIENTRY.
#
# We assume that what is left is just the function name,
# sans type and parameter information.
#
function extract_name( s ) {
    ret = s;
    sub( "^.*OGAPIENTRY[ ]*", "", ret );
    sub( "[ ]*\\(.*$", "", ret );
    return ret;
}

# The "function name" was accumulated as everything from
# the */ up to the {.  Given the function "name" data,
# we extract the parameters by stripping off:
#
# # Everything up to and including the first parentheses.
#
# # Everything after the last parentheses.
#
# We then convert comas into quoted spaces, so that the
# *roff formatter can arrange things properly.  (We really
# should provide leading and trailing quote marks, ", too,
# as well as embedded ones.  Oh well...)
#
function extract_args_list(s) {
    ret = s;
    gsub( "^.*OGAPIENTRY[ ]*[a-zA-Z0-9_]*[ ]*", "", ret );
    gsub( "^[ ]*\\([ ]*", "", ret );
    gsub( "[ ]*\\)[ ]*$", "", ret );
    gsub( "[ ]*,[ ]*", "\" \"", ret );
    return ret;
}

# This breaks down \see entries using some common
# English rules about how lists are written.
# It removes function-marking parenthesis pairs,
# and then rewrites the set of references as
# section-3 cross references.  This is returned
# to the caller.
#
function extract_xref(s) {
    ret = s;
    gsub( ", and", " 3\n.Xr ", ret );
    gsub( ", ", " 3\n.Xr ", ret );
    gsub( " and ", " 3\n.Xr ", ret );
    gsub( "\\(\\)", "", ret );
    return ".Xr " ret " 3";
}

# This code does not handle bulleted or enumerated lists
# very well, since it works by regular expressions.
# However, the result is acceptable with -mdoc in my
# test cases.
#
# Part of our problem is that we have only a peephole
# view here: Just one line, more or less, from the
# Doxygenated source file.  But writing some kinds
# of output would be easier if we could look readily
# at the previous line.  We can certainly do better
# if it becomes an issue.
#
function extract_prose(s) {
    ret = s;
    # To allow \\<symbol> for symbols that have Doxygen
    # meaning as \<symbol>, and to support that Doxygen
    # meaning (e.g., \n) while allowing documentation to
    # contain the original text (e.g., \\n), we do a nasty
    # hack: Replace \\ with an unlikely string, up-front,
    # then convert back at the other end of the processing.
    #
    gsub( "\\\\\\\\", "this never happens anywhere in dox.awk-processed text", ret );
    gsub( "\\\\n ", "\n.br\n", ret );
    gsub( "\\\\n$", "\n.br\n", ret );
    gsub( "^[ ]*", "", ret );
    gsub( "[ ]*$", "", ret );
    gsub( "^$", ".Pp", ret );
    sub( "^-#", " #", ret );
    sub( "^-", " -", ret );
    gsub( "<b>", "\n.Bf Em\n ", ret );
    gsub( "</b>", "\n.Ef\n ", ret );
    gsub( "<i>", "\n.Bf Li\n ", ret );
    gsub( "</i>", "\n.Ef\n ", ret );        
    # Replace "\a <sym>" with  "\n.Bf Sy\n <sym>\n.Ef\n "
    #
    count = split( ret, rets, "\\\\a " );
    sret = rets[ 1 ];
    ep_i = 2;
    while( ep_i <= count )
    {
        sub( "^[ ]*", "", rets[ ep_i ] );
        sub( "( |$)", "\n.Ef\n ", rets[ ep_i  ] );
        sret = sret "\n.Bf Sy\n " rets[ ep_i ];
        ++ep_i;
    }
    ret = sret;
    gsub( "this never happens anywhere in dox.awk-processed text", "\\\\", ret );
    return ret;
}

function extract_param(s) {
    ret = extract_prose( s );
    sub( " ", "\n.Ef\n ", ret );
    ret = ".Pp\n.Bf Em\n " ret;
    return ret;
}

# Originally created to strip '*' from '*OGAPIENTRY'.  Will make more
# generic if and when it seems appropriate.  (^&
#
function just_symbol(s) {
    ret = s;
    gsub( "\\*", "", ret );
    return ret;
}

# Handles the case of an open brace.  We assume that this means
# we are entering a function and at that point dump everything that
# we learned about the function from the header comment.
#
# (Alternative: We could do this at close-brace, which would allow
#  us to continue gathering \note, \bug, etc., annotations.)
#
/^{/{
    if( "doxy-func-name" == state )
    {
        func_words = split( func_name, func_word );
        fn = extract_name( func_name );
        file = path_to_dump_to fn ".3";
        print file;
        print ".\\\" Copyright 2004, the OpenGLUT contributors" >file
        print ".Dt " toupper(fn) " 3 LOCAL"                     >>file;
        #print ".Os "                                            >>file;
        #print ".Dd March 23, 2004"                              >>file;
        print ".Dd"                                             >>file;
        print ".Sh NAME"                                        >>file;
        print ".Nm " fn                                         >>file;
        print ".Nd " brief                                      >>file;
        print ".Sh LIBRARY"                                     >>file;
        print "OpenGLUT"  " - " group                           >>file;
        #for( i = 0; i < params; ++i )
        #    print "  param " i ": " param [i];
        print ".Sh SYNOPSIS"                                    >>file;
        print ".In openglut.h"                                  >>file;
        printf( ".Ft" )                                         >>file;
        for( i = 0;
            (i < func_words) && ("OGAPIENTRY" != just_symbol(func_word[i]));
            ++i )
            printf( " %s", func_word [i] )                      >>file;
        print ""                                                >>file;
        printf( ".Fn %s \"", extract_name( func_name ))         >>file;
        print extract_args_list( func_name ) "\""               >>file;
        if( params > 0 )
        {
            print ".Sh PARAMETERS"                              >>file;
            for( i = 0; i < params; ++i )
                print extract_param( param[ i ] )               >>file;
        }
        print ".Sh DESCRIPTION"                                 >>file;
        for( i = 0; i < docs; ++i )
            print extract_prose( doc [i] )                      >>file;
        if( notes > 0 )
        {
            print ".Sh CAVEATS"                                 >>file;
            for( i = 0; i < notes; ++i )
            {
                print extract_prose( note [i] )                 >>file;
                print extract_prose( "" )                       >>file;
            }
        }
        if( todos > 0 )
        {
            print ".Sh TODO"                                    >>file;
            for( i = 0; i < todos; ++i )
            {
                print extract_prose( todo [i] )                 >>file;
                print extract_prose( "" )                       >>file;
            }
        }
        if( authors > 0 )
        {
            print ".Sh AUTHORS"                                 >>file;
            # Use .An macro as a prefix?
            #
            for( i = 0; i < authors; ++i )
            {
                print extract_prose( author [i] )               >>file;
                print extract_prose( "" )                       >>file;
            }
        }
        if( bugs > 0 )
        {
            print ".Sh BUGS"                                    >>file;
            for( i = 0; i < bugs; ++i )
            {
                print extract_prose( bug [i] )                  >>file;
                print extract_prose( "" )                       >>file;
            }
        }
        if( sees > 0 )
        {
            print ".Sh SEE ALSO"                                >>file;
            for( i = 0; i < sees; ++i )
                printf extract_xref( see[i] )                   >>file;
            print ""                                            >>file;
        }
        print ".fl" >>file;
        print ".sp 3"                                           >>file;
        close( file );
    }
    init( );
    line_state = "handled";
}

# Default line handler.
#
# We use a line_state variable to tell us if any of the above
# line-handlers acted on the current line.  If no one else did,
# we analyze the line and attempt to handle it.
#
# The common case in a Doxygen comment block is to just add the
# line to a list of "doc" lines.
#
# However, we also handle, here, continued \brief, \note, and
# other types.
#
{
    stripped = $0;
    sub("^[ ]*", "", stripped );
    sub("[ ]*$", "", stripped );
    if( "unhandled" == line_state )
    {
        if( "doxy-func" == state )
        {
            doc [docs] = $0;
            ++docs;
        }
        if( "doxy-func-name" == state )
        {
            func_name = func_name " " stripped;
        }
        if( "doxy-func-brief" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else 
            {
                brief = brief " " stripped;
            }
        }
        if( "doxy-func-note" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                note [notes-1] = note[notes-1] " " stripped;
            }
        }
        if( "doxy-func-bug" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                bug [bugs-1] = bug[bugs-1] " " stripped;
            }
        }
        if( "doxy-func-param" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                param [params-1] = param[params-1] " " stripped;
            }
        }
        if( "doxy-func-see" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                see [sees-1] = see[sees-1] " " stripped;
            }
        }
        if( "doxy-func-todo" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                todo [todos-1] = todo[todos-1] " " stripped;
            }
        }
        if( "doxy-func-author" == state )
        {
            if( "" == stripped )
            {
                state = "doxy-func";
            }
            else
            {
                author [authors-1] = author[authors-1] " " stripped;
            }
        }
    }
    line_state = "unhandled";
}
