/*
 *  --- Revised 3-Clause BSD License ---
 *  Copyright (C) 2016-2019, SEMTECH (International) AG.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *      * Redistributions of source code must retain the above copyright notice,
 *        this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 *      * Neither the name of the copyright holder nor the names of its contributors
 *        may be used to endorse or promote products derived from this software
 *        without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL SEMTECH BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(CFG_prog_genkwcrcs)

#include <stdio.h>
#include "uj.h"

#define P1 257
#define P2 65537
#define SC (32-8)

#define FINISH_CRC(crc)    X((crc) ? (crc) : 1)
#define UPDATE_CRC(crc, c) X((((crc)>>SC)*P2) ^ ((crc)*P1) ^ ((c)&0x7F))
#define STR(expr) CAT(XXX,expr)
#define STR2(p) #p
#define CAT(p, ...) STR2(p ##__VA_ARGS__)

ujcrc_t X(ujcrc_t x) {
    return x;
}

ujcrc_t calcCRC(const char* s) {
    ujcrc_t crc = 0;
    int c;
    while( (c=*s++) ) {
        crc = UPDATE_CRC(crc,c);
    }
    return FINISH_CRC(crc);
}

int main (int argc, char** argv) {
    argv++;
    argc--;
    ujcrc_t crcs[argc];
    for( int i=0; i<argc; i++ ) {
        ujcrc_t crc = calcCRC(argv[i]);
        crcs[i] = crc;
        for( int j=i-1; j>=0; j-- ) {
            if( crcs[j] == crc && strcmp(argv[j], argv[i]) != 0 ) {
                fprintf(stderr, "Collision: %s(0x%X) vs %s(0x%X)\n",
                        argv[i], crc, argv[j], crcs[j]);
                exit(1);
            }
        }
    }
    printf("// Auto generated by genkwcrcs - DO NOT CHANGE!\n");
    printf("#define UJ_UPDATE_CRC(crc,c) %s\n", 4+STR(UPDATE_CRC(crc,c)));
    printf("#define UJ_FINISH_CRC(crc)   %s\n", 4+STR(FINISH_CRC(crc)));
    for( int i=0; i<argc; i++ ) {
        printf("#define J_%-20s ((ujcrc_t)(0x%08X))\n", argv[i], crcs[i]);
    }
    return 0;
}

#endif // defined(CFG_prog_genkwcrcs)
