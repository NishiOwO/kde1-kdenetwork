/*
 * Copyright (c) 1983 Regents of the University of California, (c) 1998 David Faure
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "../includ.h"

class TalkConnection
{
  public:
    /** Create a talk connection.
     * @param r_addr Remote machine IP address
     * @param r_name Remote user name
     * @param l_name Local user name */
    TalkConnection(struct in_addr r_addr,
                   char * r_name,
                   char * l_name);

    /** Destructor. Closes the sockets if opened.*/
    ~TalkConnection();
    
    /** Create the sockets */
    void open_sockets();
    /** Close the sockets */
    void close_sockets();
    
    /** Methods for talking to remote daemon */
    void ctl_transact(int type, int id_num);
    int look_for_invite();
    
    /** Connect with address given back in response to LOOK_UP. */
    int connect();
    /** Prepare to accept a connection from another talk client */
    void listen();
    /** Accept a connection from another talk client */
    int accept();
    
    /** Exchange the first 3 characters, which are edit characters */
    void set_edit_chars();

    /** Write data into the socket, by 16 char blocks. */
    void write_banner(char * banner);

    // Methods to retrieve some information
    /** Returns the erase char used by the caller. */
    char get_char_erase() { return char_erase; }
    /** Returns the caller's name. */
    char * get_caller_name() { return msg.r_name;}
    /** Returns socket, for reading or writing */
    int get_sockt() { return sockt; }
    /** Returns response buffer */
    NEW_CTL_RESPONSE * getResponse() { return &response; }
    /** Returns connection socket address. For FWT. */
    const struct sockaddr get_addr() { return msg.addr; }

    // Methods to cheat with this talk connection
    // Used by the forwarding machine
    void set_addr(const struct sockaddr * addr);
    void set_ctl_addr(const struct sockaddr * ctl_addr);
    
    /** Prints the system error message in the log and exits the current thread */
    static void p_error(const char * str);

  protected:
    /** Basic initialisation of the following fields */
    void init();
    
    /* inet addresses of the two machines */
    struct  in_addr my_machine_addr;
    struct  in_addr his_machine_addr;

    u_short daemon_port;    // port number of the talk daemon
    
    int     ctl_sockt;
    int     sockt;

    NEW_CTL_MSG msg; // holds interesting data
    NEW_CTL_RESPONSE response; // only convenience structure for responses

    char char_erase;
};
