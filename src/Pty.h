/*
    This file is part of Konsole, KDE's terminal emulator. 
    
    Copyright (C) 2007 by Robert Knight <robertknight@gmail.com>
    Copyright (C) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

#ifndef PTY_H
#define PTY_H

// Qt
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtCore/QList>

// KDE
#include <K3Process>

namespace Konsole
{

/**
 * The Pty class is used to start the terminal process, 
 * send data to it, receive data from it and manipulate 
 * various properties of the pseudo-teletype interface
 * used to communicate with the process.
 *
 * To use this class, construct an instance and connect
 * to the sendData slot and receivedData signal to
 * send data to or receive data from the process.
 *
 * To start the terminal process, call the start() method
 * with the program name and appropriate arguments. 
 */
class Pty: public K3Process
{
Q_OBJECT

  public:
    
    /** 
     * Constructs a new Pty.
     * 
     * Connect to the sendData() slot and receivedData() signal to prepare
     * for sending and receiving data from the terminal process.
     *
     * To start the terminal process, call the run() method with the 
     * name of the program to start and appropriate arguments.
     */
    Pty();
    ~Pty();

    /**
     * Starts the terminal process.  
     *
     * Returns 0 if the process was started successfully or non-zero
     * otherwise.
     *
     * @param program Path to the program to start
     * @param arguments Arguments to pass to the program being started
     * @param term Specifies the value of the TERM environment variable
     * in the process's environment.
     * @param winid Specifies the value of the WINDOWID environment variable
     * in the process's environment.
     * @param addToUtmp Specifies whether a utmp entry should be created for
     * the pty used.  See K3Process::setUsePty() 
     * @param dbusService Specifies the value of the KONSOLE_DBUS_SERVICE 
     * environment variable in the process's environment.
     * @param dbusSession Specifies the value of the KONSOLE_DBUS_SESSION
     * environment variable in the process's environment. 
     */
    int start( const QString& program, 
               const QStringList& arguments, 
               const QString& term, 
               ulong winid, 
               bool addToUtmp,
               const QString& dbusService,
               const QString& dbusSession
             );

    /** TODO: Document me */
    void setWriteable(bool writeable);

    /** 
     * Enables or disables Xon/Xoff flow control.
     *
     * See KPty::setXonXoff()
     */
    void setXonXoff(bool on);

    /** 
     * Sets the size of the window (in lines and columns of characters) 
     * used by this teletype.
     */
    void setWindowSize(int lines, int cols);

    /** TODO Document me */
    void setErase(char erase);

    /**
     * Returns the process id of the teletype's current foreground
     * process.  This is the process which is currently reading
     * input sent to the terminal via. sendData()
     *
     * If there is a problem reading the foreground process group,
     * 0 will be returned.
     */
    int foregroundProcessGroup() const;
   
    /** TODO: Document me */ 
    bool bufferFull() const { return _bufferFull; }

  public slots:

    /**
     * Put the pty into UTF-8 mode on systems which support it.
     *
     * See KPty::setUtf8Mode()
     */
    void setUtf8Mode(bool on);

    /**
     * Suspend or resume processing of data from the standard 
     * output of the terminal process.
     *
     * See K3Process::suspend() and K3Process::resume()
     *
     * @param lock If true, processing of output is suspended,
     * otherwise processing is resumed.
     */
    void lockPty(bool lock);
    
    /** 
     * Sends data to the process currently controlling the 
     * teletype ( whose id is returned by foregroundProcessGroup() )
     *
     * @param buffer Pointer to the data to send.
     * @param length Length of @p buffer.
     */
    void sendData(const char* buffer, int length);

  signals:

    /**
     * Emitted when the terminal process terminates.
     *
     * @param exitCode The status code which the process exited with.
     */
    void done(int exitCode);

    /**
     * Emitted when a new block of data is received from
     * the teletype.
     *
     * @param buffer Pointer to the data received.
     * @param length Length of @p buffer
     */
    void receivedData(const char* buffer, int length);
    
    /**
     * TODO: Document me  
     */
    void bufferEmpty();

  private slots:
    
    // called when terminal process exits
    void donePty();
    // called when data is received from the terminal process 
    void dataReceived(K3Process*, char* buffer, int length);
    // sends the first enqueued buffer of data to the
    // terminal process
    void doSendJobs();
    // called when the terminal process is ready to
    // receive more data
    void writeReady();

  private:
    // enqueues a buffer of data to be sent to the 
    // terminal process
    void appendSendJob(const char* buffer, int length);
   
    // a buffer of data in the queue to be sent to the 
    // terminal process 
    class SendJob {
	public:
      		SendJob() {}
      		SendJob(const char* b, int len) : buffer(len)
		{
			memcpy( buffer.data() , b , len );
        }
	
		const char* data() const { return buffer.constData(); }
		int length() const { return buffer.size(); }	
	private:
      		QVector<char> buffer;
    };

    QList<SendJob> _pendingSendJobs;
    bool _bufferFull;
};

}

#endif // PTY_H
