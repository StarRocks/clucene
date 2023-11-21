/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/_ApiHeader.h"
#include "LanguageBasedAnalyzer.h"
#include "CLucene/analysis/cjk/CJKAnalyzer.h"
#include "CLucene/analysis/Analyzers.h"
#include "CLucene/analysis/standard/StandardTokenizer.h"
#include "CLucene/analysis/standard/StandardFilter.h"
#include "CLucene/snowball/SnowballFilter.h"

CL_NS_USE(util)
CL_NS_USE2(analysis,cjk)
CL_NS_USE2(analysis,standard)
CL_NS_USE2(analysis,snowball)

CL_NS_DEF(analysis)

LanguageBasedAnalyzer::LanguageBasedAnalyzer(const TCHAR* language, bool stem)
{
  if ( language == NULL )
	 _tcsncpy(lang,LUCENE_BLANK_STRING,100);
  else
	 _tcsncpy(lang,language,100);
	this->stem = stem;
}
LanguageBasedAnalyzer::~LanguageBasedAnalyzer(){
  if (_save_streams) {
         _CLDELETE(_save_streams)
  }
}
void LanguageBasedAnalyzer::setLanguage(const TCHAR* language){
	_tcsncpy(lang,language,100);
}
void LanguageBasedAnalyzer::setStem(bool stem){
	this->stem = stem;
}
TokenStream* LanguageBasedAnalyzer::reusableTokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader) {
        if (!_save_streams) {
         _save_streams = _CLNEW SavedStreams();
         if (_tcscmp(lang, _T("cjk")) == 0) {
           _save_streams->tokenStream = _CLNEW CL_NS2(analysis, cjk)::CJKTokenizer(reader);
           return _save_streams->tokenStream;
         } else {
           CL_NS(util)::BufferedReader* bufferedReader = reader->__asBufferedReader();

           if (bufferedReader) {
             _save_streams->tokenStream = _CLNEW StandardTokenizer(
                 _CLNEW CL_NS(util)::FilteredBufferedReader(reader, false), true);
           } else {
             _save_streams->tokenStream = _CLNEW StandardTokenizer(bufferedReader);
           }

           _save_streams->filteredTokenStream = _CLNEW StandardFilter(_save_streams->tokenStream, true);
           if (stem) {
             _save_streams->filteredTokenStream = _CLNEW SnowballFilter( _save_streams->filteredTokenStream, lang, true);
           }
           _save_streams->filteredTokenStream =
               _CLNEW LowerCaseFilter(_save_streams->filteredTokenStream, true);
           return _save_streams->filteredTokenStream;
         }
        } else {
         _save_streams->tokenStream->reset(reader);
         return _save_streams -> filteredTokenStream
                    ? _save_streams -> filteredTokenStream
                    : _save_streams -> tokenStream;
        }
}
TokenStream* LanguageBasedAnalyzer::tokenStream(const TCHAR* fieldName, Reader* reader) {
	TokenStream* ret = NULL;
	if ( _tcscmp(lang, _T("cjk"))==0 ){
		ret = _CLNEW CL_NS2(analysis,cjk)::CJKTokenizer(reader);
	}else{
    BufferedReader* bufferedReader = reader->__asBufferedReader();
    if ( bufferedReader == NULL )
      ret =  _CLNEW StandardTokenizer( _CLNEW FilteredBufferedReader(reader, false), true );
    else
      ret = _CLNEW StandardTokenizer(bufferedReader);

		ret = _CLNEW StandardFilter(ret,true);

		if ( stem )
			ret = _CLNEW SnowballFilter(ret,lang, true); //todo: should check whether snowball supports the language

		if ( stem ) //hmm... this could be configured seperately from stem
			ret = _CLNEW ISOLatin1AccentFilter(ret, true); //todo: this should really only be applied to latin languages...

                //lower case after the latin1 filter
		ret = _CLNEW LowerCaseFilter(ret,true);
	}
        //todo: could add a stop filter based on the language - need to fix the stoplist loader first

	return ret;
}

CL_NS_END
