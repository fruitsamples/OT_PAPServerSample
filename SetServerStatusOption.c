/*	File:		SetServerStatusOption.c	Contains:		Written by: 		Copyright:	Copyright � 1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				7/22/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*/#include "OpenTransport.h"			// open transport files			#include "OpenTptAppleTalk.h"#include <Memory.h>/***	prototype declaration*/extern OSStatus DoSetServerStatusOption(EndpointRef ep, char *statusStr);/*	global variables	The nature of the following call is such that you'll want to set the option 	when the endpoint is in asynch mode.  To handle the asynch case, you must	declare the global gOptionCompleted.  When an option management call is made	gOptionCompleted is set to zero.  When the T_OPTMGMTCOMPLETE call is handled	by the endpoint handler, then gOptionCompleted is set to 1.  Note that the	sample spin loops until gOptionCompleted is non-zero.  As such, the 	DoSetServerStatusOption call cannot be made at interrupt time or from a deferred	task.  The DoSetServerStatusOption can only be made at system task time.		Ensure that gOptionCompleted is used to flag only one outstanding Option 	Management call at a time.	*/extern UInt32	gOptionCompleted;/*	input parameters	ep - endpoint on which to set the status string option	statusstr - C style string of the status string to set		function result - kOTNoError indicates that the option was successfully negotiated		if the result is negative, then this is the result returned by the call to		OTOptionManagement.  If the result is positive, then this is the value of 		the status field on return from the call to */OSStatus DoSetServerStatusOption(EndpointRef ep, char *statusStr){	UInt8		buf[kOTOptionHeaderSize+256];	// define buffer to allow for 												// up to a 256 char string	TOption*	opt;							// option ptr to make items easier to access	TOptMgmt	req;	TOptMgmt	ret;	UInt32 		statusStrLen;	OSStatus	err;	Boolean		isAsync = false;		if (OTIsSynchronous(ep) == false)			// check whether ep sync or not	{		isAsync = true;		gOptionCompleted = 0;	}		statusStrLen = OTStrLength(statusStr);	// get the length of the C string			opt = (TOption*)buf;					// set option ptr to buffer	req.opt.buf	= buf;	req.opt.len	= kOTOptionHeaderSize + statusStrLen;	req.flags	= T_NEGOTIATE;				// negotiate for serverstatus option	ret.opt.buf = buf;	ret.opt.maxlen = kOTOptionHeaderSize+256;		opt->level	= ATK_PAP;					// dealing with tpi	opt->name	= OPT_SERVERSTATUS;	opt->len	= kOTOptionHeaderSize + statusStrLen;	opt->status = 0;		// set the serverStr into the value field	BlockMove(statusStr, (Ptr)&opt->value, statusStrLen);	err = OTOptionManagement(ep, &req, &ret);		if ((isAsync == true) && (err == kOTNoError))	{		while (gOptionCompleted == 0)		{			// spin in this null loop waiting for the option call to complete.		}	}			// if no error then return the option status value	if (err == kOTNoError)	{		if (opt->status != T_SUCCESS)			err = opt->status;		else			err = kOTNoError;	}			return err;}