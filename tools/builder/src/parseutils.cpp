/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include <antlr3.h>

#include "parseutils.h"
#include "exception.h"



void parseError(const char* errorName, int line)
{
    THROW_EXCEPT(Exception::ERR_PARSE,
                 String(errorName),
                 "",
                 line);
}

void displayRecognitionError (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
    // Adopted code from https://github.com/antlr/antlr3/blob/master/runtime/C/src/antlr3baserecognizer.c

    pANTLR3_PARSER parser;
    pANTLR3_TREE_PARSER tparser;
    pANTLR3_INT_STREAM is;
    pANTLR3_EXCEPTION ex;
    pANTLR3_COMMON_TOKEN theToken;
    pANTLR3_BASE_TREE theBaseTree;
    pANTLR3_COMMON_TREE theCommonTree;

    // Retrieve some info for easy reading.
    ex = recognizer->state->exception;
    String fileName;
    // See if there is a 'filename' we can use
    if	(ex->streamName != NULL)
    {
        pANTLR3_STRING ftext;
        ftext = ex->streamName->to8(ex->streamName);
        fileName.assign((const char*)ftext->chars);
    }

    int line = recognizer->state->exception->line;
    StringStream ss;
    ss << (const char*) (recognizer->state->exception->message);

    // How we determine the next piece is dependent on which thing raised the
    // error.
    pANTLR3_STRING ttext;
    switch	(recognizer->type)
    {
    case ANTLR3_TYPE_PARSER:
        // Prepare the knowledge we know we have
        parser = (pANTLR3_PARSER) (recognizer->super);
        tparser = NULL;
        is = parser->tstream->istream;
        theToken = (pANTLR3_COMMON_TOKEN)(recognizer->state->exception->token);
        ttext = theToken->toString(theToken);
        ss << ", at offset " << recognizer->state->exception->charPositionInLine;
        if (theToken != NULL)
        {
            if (theToken->type == ANTLR3_TOKEN_EOF)
                ss << ", at <EOF>";
            else
                // Guard against null text in a token
                ss << "\n near " << (ttext == NULL ? "<no text for the token>" : (const char*)ttext->chars) << "\n ";
        }
        break;

    case ANTLR3_TYPE_TREE_PARSER:
        tparser = (pANTLR3_TREE_PARSER) (recognizer->super);
        parser = NULL;
        is = tparser->ctnstream->tnstream->istream;
        theBaseTree = (pANTLR3_BASE_TREE)(recognizer->state->exception->token);
        ttext = theBaseTree->toStringTree(theBaseTree);
        if (theBaseTree != NULL)
        {
            theCommonTree = (pANTLR3_COMMON_TREE) theBaseTree->super;
            if	(theCommonTree != NULL)
            {
                theToken = (pANTLR3_COMMON_TOKEN) theBaseTree->getToken(theBaseTree);
            }
            ss << ", at offset " << theBaseTree->getCharPositionInLine(theBaseTree);
            ss << ", near " << (const char*)ttext->chars;
        }
        break;

    default:
        std::cerr << "Base recognizer function displayRecognitionError called by unknown parser type - provide override for this function\n";
        return;
        break;
    }

    // Although this function should generally be provided by the implementation, this one
    // should be as helpful as possible for grammar developers and serve as an example
    // of what you can do with each exception type. In general, when you make up your
    // 'real' handler, you should debug the routine with all possible errors you expect
    // which will then let you be as specific as possible about all circumstances.
    //
    // Note that in the general case, errors thrown by tree parsers indicate a problem
    // with the output of the parser or with the tree grammar itself. The job of the parser
    // is to produce a perfect (in traversal terms) syntactically correct tree, so errors
    // at that stage should really be semantic errors that your own code determines and handles
    // in whatever way is appropriate.
    switch (ex->type)
    {
    case ANTLR3_UNWANTED_TOKEN_EXCEPTION:
        // Indicates that the recognizer was fed a token which seesm to be
        // spurious input. We can detect this when the token that follows
        // this unwanted token would normally be part of the syntactically
        // correct stream. Then we can see that the token we are looking at
        // is just something that should not be there and throw this exception.
        if	(tokenNames == NULL)
            ss << " : Extraneous input...";
        else
        {
            if	(ex->expecting == ANTLR3_TOKEN_EOF)
                ss << " : Extraneous input - expected <EOF>\n";
            else
                ss << " : Extraneous input - expected " << tokenNames[ex->expecting] << " ...\n";
        }
        break;

    case ANTLR3_MISSING_TOKEN_EXCEPTION:
        // Indicates that the recognizer detected that the token we just
        // hit would be valid syntactically if preceeded by a particular
        // token. Perhaps a missing ';' at line end or a missing ',' in an
        // expression list, and such like.
        if	(tokenNames == NULL)
            ss << " : Missing token (" << ex->expecting << ")...\n";
        else
        {
            if	(ex->expecting == ANTLR3_TOKEN_EOF)
                ss << " : Missing <EOF>\n";
            else
                ss << " : Missing " << tokenNames[ex->expecting] << " \n";
        }
        break;

    case ANTLR3_RECOGNITION_EXCEPTION:
        // Indicates that the recognizer received a token
        // in the input that was not predicted. This is the basic exception type
        // from which all others are derived. So we assume it was a syntax error.
        // You may get this if there are not more tokens and more are needed
        // to complete a parse for instance.
        ss << " : syntax error...\n";
        break;

    case ANTLR3_MISMATCHED_TOKEN_EXCEPTION:
        // We were expecting to see one thing and got another. This is the
        // most common error if we coudl not detect a missing or unwanted token.
        // Here you can spend your efforts to
        // derive more useful error messages based on the expected
        // token set and the last token and so on. The error following
        // bitmaps do a good job of reducing the set that we were looking
        // for down to something small. Knowing what you are parsing may be
        // able to allow you to be even more specific about an error.
        if (tokenNames == NULL)
            ss << " : syntax error...\n";
        else
        {
            if	(ex->expecting == ANTLR3_TOKEN_EOF)
                ss << " : expected <EOF>\n";
            else
                ss << " : expected " << tokenNames[ex->expecting] << " ...\n";
        }
        break;

    case ANTLR3_NO_VIABLE_ALT_EXCEPTION:
        // We could not pick any alt decision from the input given
        // so god knows what happened - however when you examine your grammar,
        // you should. It means that at the point where the current token occurred
        // that the DFA indicates nowhere to go from here.
        ss << " : cannot match to any predicted input...\n";
        break;

    case ANTLR3_MISMATCHED_SET_EXCEPTION:
        {
            ANTLR3_UINT32 count;
            ANTLR3_UINT32 bit;
            ANTLR3_UINT32 size;
            ANTLR3_UINT32 numbits;
            pANTLR3_BITSET errBits;
            // This means we were able to deal with one of a set of
            // possible tokens at this point, but we did not see any
            // member of that set.

            ss << " : unexpected input...\n expected one of : ";
            // What tokens could we have accepted at this point in the
            // parse?
            count = 0;
            errBits = antlr3BitsetLoad	(ex->expectingSet);
            numbits = errBits->numBits	(errBits);
            size = errBits->size	(errBits);

            if (size > 0)
            {
                // However many tokens we could have dealt with here, it is usually
                // not useful to print ALL of the set here. I arbitrarily chose 8
                // here, but you should do whatever makes sense for you of course.
                // No token number 0, so look for bit 1 and on.

                for	(bit = 1; bit < numbits && count < 8 && count < size; bit++)
                {
                    // TODO: This doesn;t look right - should be asking if the bit is set!!
                    if (tokenNames[bit])
                    {
                        ss << (count > 0 ? ", " : "") << tokenNames[bit];
                        count++;
                    }
                }
                ss << "\n";
            }
            else
            {
                std::cerr << "Actually dude, we didn't seem to be expecting anything here, or at least\n";
                std::cerr << "I could not work out what I was expecting, like so many of us these days!\n";
            }
        }
        break;

    case ANTLR3_EARLY_EXIT_EXCEPTION:
        // We entered a loop requiring a number of token sequences
        // but found a token that ended that sequence earlier than
        // we should have done.
        ss << " : missing elements...\n";
        break;

    default:
        // We don't handle any other exceptions here, but you can
        // if you wish. If we get an exception that hits this point
        // then we are just going to report what we know about the
        // token.
        ss << " : syntax not recognized...\n";
        break;
    }

    THROW_EXCEPT(Exception::ERR_PARSE,
                 ss.str(),
                 fileName,
                 line);
}
