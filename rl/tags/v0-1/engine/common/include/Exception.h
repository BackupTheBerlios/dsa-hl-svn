/* This source file is part of Rastullahs Lockenpracht.
 * Copyright (C) 2003-2005 Team Pantheon. http://www.team-pantheon.de
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the Clarified Artistic License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Clarified Artistic License for more details.
 *
 *  You should have received a copy of the Clarified Artistic License
 *  along with this program; if not you can get it here
 *  http://www.jpaulmorrison.com/fbp/artistic2.htm.
 */

#ifndef __RL_EXCEPTION_H__
#define __RL_EXCEPTION_H__
/**
 * \file Exception.h
 * Einige Definitionen fuer Asserts und Exception Handling
 * und Meldungsausgabe.
 */

#include "RastullahPrerequisites.h"
#include <string>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#  include <Windows.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   // visual studio >= 7.0 oder gar kein VS
#  if !defined(_MSC_VER) || (_MSC_VER >= 1300)
#    define __FUNC__ __FUNCTION__
#  else
     // VS < 7.0 kennt kein __FUNCTION__
#    define __FUNC__ ""	  
#  endif
#else
#  define __FUNC__ __PRETTY_FUNCTION__
#endif

#ifndef RL_NO_EXCEPTIONS
   /**@brief Makro fuer das Werfen von Exceptions.
    *
    * Benutzung:
    * @code
    * Throw(Exception, "Ein Fehler ist aufgetreten.");
    * @endcode
    * @param exception Klassenname der zu werfenden Exception.
    * @param msg Die Ursache fuer die Exception.
    */
#  define Throw(exception, msg) \
   throw(exception(msg, __FILE__, __FUNC__, __LINE__))
   /**@brief Makro zur Absicherung von Annahmen.
    *
    * Wenn der Ausdruck \c test zu \c false ausgewertet wird, wird eine
    * \c AssertionFailedException geworfen mit \c msg als Grund.
    *
    * @param test Boolescher Ausdruck der ausgewertet werden soll.
    * @param msg Meldung, die als Beschreibung an die Exception gegeben
    * werden soll, falls \c test \c false ist.
    */
#  define RlAssert(test, msg) if(!(test)) Throw(AssertionFailedError, msg)
#  define RlAssert1(test) RlAssert(test, #test)
   /**@brief Makro zur Absicherung von Annahmen.
    *
    * Wirft eine \c AssertionFailedException. N?tzlich um
    * Zust?nde die nicht auftreten sollen abzusichern, etwa in
    * \c switch Bl?cken f?r den \c default, falls es keinen
    * solchen geben soll.
    *
    * @param test Boolescher Ausdruck der ausgewertet werden soll.
    * @param msg Meldung, die als Beschreibung an die Exception gegeben
    * werden soll, falls \c test \c false ist.
    */
#  define RlFail(msg) Throw(AssertionFailedError, msg)
#endif
#ifdef RL_NO_ASSERTS
#  define RlAssert(test, msg)
#  define RlFail(msg)
#endif

namespace rl {

    /**@brief Das ist die Basis-Exceptionklasse fuer das Projekt.
    *
    * Sie ist entgegen den Richtlinien inline definiert, weil sie keinem
    * Projekt zugehoert und nicht exportiert wird.
    * So ist es dann am einfachsten.
    *
    * @warning Nie direkt ueber throw() werfen,
    *    sondern mittels des Makros Throw().
    */
   class Exception {

   protected:
      std::string mMessage;
      std::string mFile;
      std::string mFunction;
      int mLine;

   public:
      Exception(const std::string& message,
         const std::string& file, const std::string& function, int line) :
      mMessage(message), mFile(file), mFunction(function), mLine(line) {}
      
      Exception(const Exception& rhs) :
         mMessage(rhs.mMessage),
         mFile(rhs.mFile),
         mFunction(rhs.mFunction),
         mLine(rhs.mLine) {}

      virtual ~Exception() {}

      Exception& operator=(const Exception& rhs)
      {
         if (this != &rhs) {
            mMessage = rhs.mMessage;
            mFile = rhs.mFile;
            mFunction = rhs.mFunction;
            mLine = rhs.mLine;
         }
         return *this;
      }      

      virtual std::string getMessage() { return mMessage; }
      virtual std::string getFile() { return mFile; }
      virtual std::string getFunction() { return mFunction; }
      virtual std::string getType() { return "Exception"; }
      virtual int getLine() { return mLine; }

      virtual std::string toString()
      {
         char* lineStr = new char[2048];
         _snprintf(lineStr, 2048, "In Datei %s, Zeile %d, Funktion %s\n\
trat folgende Ausnahme vom Typ %s auf\n\
-----------------------------------------------------------------------\n\
%s",
            getFile().c_str(),
            getLine(),
            getFunction().c_str(),
            getType().c_str(),
            getMessage().c_str() );
         return std::string(lineStr);
      }
   };

   class Error : public Exception {
   public:
      Error(const std::string& message, const std::string& file,
         const std::string& function, int line)
         : Exception(message, file, function, line) {}

      Error(const Error& rhs) : Exception(rhs) {}

      virtual std::string getType() { return "Error"; }

   };

   class RuntimeException : public Exception {
   public:
      RuntimeException(const std::string& message, const std::string& file,
         const std::string& function, int line)
         : Exception(message, file, function, line) {}

      RuntimeException(const RuntimeException& rhs) : Exception(rhs) {}

      virtual std::string getType() { return "RuntimeException"; }

   };

   class NullPointerException : public RuntimeException {
   public:
      NullPointerException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      NullPointerException(const NullPointerException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "NullPointerException"; }

   };

   class InvalidArgumentException : public RuntimeException {
   public:
      InvalidArgumentException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      InvalidArgumentException(const InvalidArgumentException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "InvalidArgumentException"; }

   };

   class OperationNotSupportedException : public RuntimeException {
   public:
      OperationNotSupportedException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      OperationNotSupportedException(const OperationNotSupportedException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "OperationNotSupportedException"; }

   };

   class IllegalStateException : public RuntimeException {
   public:
      IllegalStateException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      IllegalStateException(const IllegalStateException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "IllegalStateException"; }

   };

   class AssertionFailedError : public Error {
   public:
      AssertionFailedError(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : Error(message, file, function, line) {}

      AssertionFailedError(const AssertionFailedError& rhs) : Error(rhs) {}

      virtual std::string getType() { return "AssertionFailedError"; }

   };

   class ScriptInvocationFailedException : public RuntimeException {
   public:
       ScriptInvocationFailedException(const std::string& message,
           const std::string& file, const std::string& function, int line)
           : RuntimeException(message, file, function, line) {}

       ScriptInvocationFailedException(const ScriptInvocationFailedException& rhs)
               : RuntimeException(rhs) {}

       virtual std::string getType() { return "ScriptInvocationFailedException"; }
   };

   class OutOfRangeException : public RuntimeException {
   public:
      OutOfRangeException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      OutOfRangeException(const OutOfRangeException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "OutOfRangeException"; }

   };

   class DuplicateIdException : public RuntimeException {
   public:
      DuplicateIdException(const std::string& message,
         const std::string& file, const std::string& function, int line)
         : RuntimeException(message, file, function, line) {}

      DuplicateIdException(const DuplicateIdException& rhs)
         : RuntimeException(rhs) {}

      virtual std::string getType() { return "DuplicateIdException"; }
   };


   /**@brief Funktion zum ausgeben einer Nachricht an den Benutzer.
    *
    * Gibt ein Meldung an den Benutzer aus. Entweder ueber STDOUT oder
    * in einem Meldungsfenster aus.
    *
    * @todo Definition an einen passenderen Ort verschieben.
    */
   static void showMessage(const std::string& message)
   {
#     if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
         MessageBox(NULL, message.c_str(), "Meldung",
            MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
#     else
         printf(message.c_str());
#     endif
   }

   /**@brief Funktion zum ausgeben eines Fehlers an den Benutzer.
    *
    * Gibt ein Fehlermeldung an den Benutzer aus. Entweder ueber STDERR oder
    * in einem Meldungsfenster aus.
    *
    * @todo Definition an einen passenderen Ort verschieben.
    */
   static void showError(const std::string& message)
   {
#     if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
         MessageBox(NULL, message.c_str(), "Fehler",
            MB_OK | MB_ICONERROR | MB_TOPMOST);
#     else
         fprintf(stderr, message.c_str());
#     endif
   }
}
#endif
