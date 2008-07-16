/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#ifndef _lucene_search_IndexSearcher_
#define _lucene_search_IndexSearcher_


#include "Searchable.h"
CL_CLASS_DEF(store,Directory)
CL_CLASS_DEF(document,Document)
CL_CLASS_DEF(index,Term)
CL_CLASS_DEF(search,TopDocs)
CL_CLASS_DEF(search,TopFieldDocs)
CL_CLASS_DEF(search,Query)
CL_CLASS_DEF(search,Filter)
CL_CLASS_DEF(search,Sort)
CL_CLASS_DEF(search,HitCollector)
CL_CLASS_DEF(search,Explanation)
CL_CLASS_DEF(index,IndexReader)
//#include "CLucene/index/IndexReader.h"
//#include "CLucene/util/BitSet.h"
//#include "HitQueue.h"
//#include "FieldSortedHitQueue.h"

CL_NS_DEF(search)
/** Implements search over a single IndexReader.
*
* <p>Applications usually need only call the inherited {@link search(Query*)}
* or {@link search(Query*,Filter*)} methods.
*/
class CLUCENE_EXPORT IndexSearcher:public Searcher{
	CL_NS(index)::IndexReader* reader;
	bool readerOwner;

public:
	/// Creates a searcher searching the index in the named directory.
	IndexSearcher(const char* path);
  
    /// Creates a searcher searching the index in the specified directory.
    IndexSearcher(CL_NS(store)::Directory* directory);

	/// Creates a searcher searching the provided index. 
	IndexSearcher(CL_NS(index)::IndexReader* r);

	~IndexSearcher();
	    
	/// Frees resources associated with this Searcher. 
	void close();

	int32_t docFreq(const CL_NS(index)::Term* term) const;

	bool doc(int32_t i, CL_NS(document)::Document* document);
	_CL_DEPRECATED( doc(i, document) ) CL_NS(document)::Document* doc(int32_t i);

	int32_t maxDoc() const;

	TopDocs* _search(Query* query, Filter* filter, const int32_t nDocs);
	TopFieldDocs* _search(Query* query, Filter* filter, const int32_t nDocs, const Sort* sort);

	void _search(Query* query, Filter* filter, HitCollector* results);

	CL_NS(index)::IndexReader* getReader();

	Query* rewrite(Query* original);
	void explain(Query* query, int32_t doc, Explanation* ret);

	virtual const TCHAR* getClassName();
};
CL_NS_END
#endif