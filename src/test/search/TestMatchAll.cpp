/*------------------------------------------------------------------------------
 * Regression test for StarRocks-style match_all (Boolean AND of terms).
 * Exercises ConjunctionScorer::skipTo -> SegmentTermDocs -> DefaultSkipListReader.
 *------------------------------------------------------------------------------*/
#include "test.h"
#include "CheckHits.h"
#include "CLucene/analysis/Analyzers.h"
#include "CLucene/index/MergePolicy.h"
#include "CLucene/search/BooleanQuery.h"

CL_NS_USE(analysis)
CL_NS_USE(document)
CL_NS_USE(index)
CL_NS_USE(search)
CL_NS_USE(store)

static void testMatchAllSparseIntersectionEvery40(CuTest* tc) {
	const int32_t numDocs = 200;
	const int32_t bothEvery = 40;

	RAMDirectory dir;
	WhitespaceAnalyzer analyzer;
	IndexWriter writer(&dir, &analyzer, true);
	writer.setMergePolicy(_CLNEW LogDocMergePolicy());
	writer.setMaxBufferedDocs(1000);

	Document doc;
	for (int32_t i = 0; i < numDocs; i++) {
		doc.clear();
		const TCHAR* text;
		if ((i % bothEvery) == 0) {
			text = _T("termA termB");
		} else if ((i % 2) == 0) {
			text = _T("termA");
		} else {
			text = _T("termB");
		}
		doc.add(*_CLNEW Field(_T("content"), text, Field::STORE_NO | Field::INDEX_TOKENIZED));
		writer.addDocument(&doc);
	}
	writer.close();

	IndexSearcher searcher(&dir);
	Term termA(_T("content"), _T("termA"));
	Term termB(_T("content"), _T("termB"));
	assertEquals(100, searcher.getReader()->docFreq(&termA));
	assertEquals(105, searcher.getReader()->docFreq(&termB));	

	// equivalent to match_all 'termA termB'
	Term* tA = _CLNEW Term(_T("content"), _T("termA"));
	Term* tB = _CLNEW Term(_T("content"), _T("termB"));
	BooleanQuery* query = _CLNEW BooleanQuery();
	query->add(_CLNEW TermQuery(tA), true, BooleanClause::MUST);
	query->add(_CLNEW TermQuery(tB), true, BooleanClause::MUST);
	_CLDECDELETE(tA);
	_CLDECDELETE(tB);

	const int32_t expectedCount = 5;
	int32_t expected[5] = {0, 40, 80, 120, 160};
	CheckHits::checkHitCollector(tc, query, _T("content"), &searcher, expected, (size_t)expectedCount);

	_CLDELETE(query);
	searcher.close();
}

CuSuite* testMatchAll(void) {
	CuSuite* suite = CuSuiteNew(_T("CLucene MatchAll (Boolean AND) Test"));
	SUITE_ADD_TEST(suite, testMatchAllSparseIntersectionEvery40);
	return suite;
}
