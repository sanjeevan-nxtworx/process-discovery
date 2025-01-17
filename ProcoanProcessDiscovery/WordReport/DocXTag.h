#pragma once
#include "DocXTagAttributes.h"
#include <list>
#include <string>

using namespace std;
class Table;
class Paragraph;
class ParagraphRun;
struct PictureInsertionProperties;
struct PictureProperties;

class DocXTag
{
protected:
	string tagName;
	string tagValue;
	list <DocXTag *> listTags;
	list <DocXTagAttributes *> listAttributes;
	bool bFullTag;

public:
	DocXTag(string name);
	DocXTag(string name, string value);
	DocXTag(string name, bool fullTag);
	~DocXTag();
	void AddAttribute(string key, string value);
	void AddAttribute(string key, int value);
	void AddAttribute(string key, long value);
	DocXTag *AddTag(string name);
	DocXTag *AddTag(DocXTag *pTag);
	DocXTag *AddTag(string name, bool bFullTag);
	Table *AddTable();
	Paragraph *AddParagraph();
	ParagraphRun *AddParagraphRun();
	void InsertInlinePictureFromFile(ParagraphRun *pRun, struct PictureInsertionProperties *pInsProps, struct PictureProperties *pPicProps);

	void SetValue(string value);
	string GetTagString();
	bool IsFullTag();
	void SetBookmarkStart(string id, string name);
	void SetBookmarkEnd(string id);

	void AddFldChar(string text, bool bPreserve = false, bool bNoProof = false, bool bWebHidden = false);
	void AddFldValue(int val, bool bNoProof = false, bool bWebHidden = false);
	void EndFldChar(bool bNoProof = false, bool bWebHidden = false);

};

