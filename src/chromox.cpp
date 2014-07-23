

#include "mlog.h"
#include "chromox.h"

const char* KChromoSystemId = "/usr/share/grayb/conf/objspec.dtd";

ChromoMdlX::ChromoMdlX(): ChromoMdl("ChromoMdlX"), iDoc(NULL), iDocOwned(EFalse)
{
};

ChromoMdlX::~ChromoMdlX()
{
    Reset(); 
}

void *ChromoMdlX::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx) 
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

void* ChromoMdlX::Init(TNodeType aRootType)
{
    iDocOwned = ETrue;
    iDoc = xmlNewDoc((const xmlChar*) "1.0");
    string sroottype = GUri::NodeTypeName(aRootType);
    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar *) sroottype.c_str());
    xmlDocSetRootElement(iDoc, root);
    //xmlDtdPtr dtd = xmlNewDtd(iDoc, (const xmlChar*) "iobject", (const xmlChar*) KChromoSystemId, (const xmlChar*) KChromoSystemId);
    xmlDtdPtr dtd = xmlParseDTD(NULL, (const xmlChar*) KChromoSystemId );
    iDoc->extSubset = dtd;
    return root;
}

void ChromoMdlX::Reset()
{
    if (iDocOwned) {
	xmlFreeDoc(iDoc);
    }
    iDoc = NULL;
}

void* ChromoMdlX::Find(const void* aHandle, const string& aUri)
{
    GUri desuri(aUri);
    const vector<GUri::TElem>& elems = desuri.Elems();
    xmlNodePtr fres = NULL;
    xmlNodePtr res = (xmlNodePtr) aHandle;
    for (vector<GUri::TElem>::const_iterator it = desuri.Elems().begin(); it != desuri.Elems().end() && res != NULL; it++) {
	GUri::TElem elem = *it;
	while (res != NULL) {
	    if (res->type == XML_ELEMENT_NODE) {
		TNodeType type = GetType((void*) res);
		char *name = (char*) xmlGetProp(res, (const xmlChar *) GUri::NodeAttrName(ENa_Id).c_str());
		char *parent = (char*) xmlGetProp(res, (const xmlChar *) GUri::NodeAttrName(ENa_Parent).c_str());
		string eparent;
		char rel = GUri::GetExtFirstPart(elem.first, eparent);
		if (type == ENt_Node && rel == GUri::KParentSep && (eparent == GUri::KTypeAny || eparent.compare(parent) == 0) && elem.second.second.compare(name) == 0) {
		    break;
		}
	    }
	    res = res->next;
	}
	if (res != NULL) {
	    fres = res;
	    res = res->children;
	}
    }
    return fres;
}

void* ChromoMdlX::Set(const char *aFileName)
{
    xmlNode *sEnv = NULL; // Node of environment element
    xmlNode *sRoot = NULL; // Node of root element
    // Read and parse the CAE spec file
    iDoc = xmlReadFile(aFileName, NULL, XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID);
    __ASSERT(iDoc != NULL);
    // Get the node 
    sRoot = (xmlNodePtr) GetFirstChild((void *) iDoc, ENt_Node);
    iDocOwned = EFalse;
    return sRoot;
}

void* ChromoMdlX::Set(const string& aUri)
{
    xmlNodePtr res = NULL;
    xmlNode *sEnv = NULL; // Node of environment element
    xmlNode *sRoot = NULL; // Node of root element
    // Read and parse the CAE spec file
    string path;
    Chromo::GetPath(aUri, path);
    iDoc = xmlReadFile(path.c_str(), NULL, XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID);
    __ASSERT(iDoc != NULL);
    // Get the node 
    string desuri;
    Chromo::GetFrag(aUri, desuri);
    sRoot = (xmlNodePtr) GetFirstChild((void *) iDoc, ENt_Node);
    if (!desuri.empty()) {
	// Transform into rel uri if abs
	// TODO [YB] - to consider if it would be better to update Find to accept the abs uri
	GUri desurib = GUri(desuri);
	GUri::const_elem_iter it = desurib.Elems().begin();
	if (it->second.second.empty()) {
	    it++;
	}
	desuri = desurib.GetUri(it);
	res = (xmlNodePtr) Find(sRoot, desuri); 
    }
    else {
	res = sRoot;
    }
    iDocOwned = EFalse;
    return res;
}

void* ChromoMdlX::Set(const void* aHandle)
{
    xmlNodePtr node = xmlDocCopyNode((xmlNodePtr) aHandle, iDoc, 1);
    return node;
}

TNodeType ChromoMdlX::GetType(const string& aId)
{
    return GUri::NodeType(aId);
}

TNodeType ChromoMdlX::GetType(const void* aHandle)
{
    TNodeType res = ENt_Unknown;
    xmlNodePtr node = (xmlNodePtr) aHandle;
    if (node->name != NULL) {
	const char* type_name = (const char*) node->name;
	res = GUri::NodeType(type_name);
    }
    return res;
}

void *ChromoMdlX::GetFirstChild(const void *aHandle, TNodeType aType)
{
    xmlNodePtr node = (xmlNodePtr) aHandle;
    __ASSERT(node != NULL);
    xmlNodePtr res = node->children;
    if (res != NULL) {
	TNodeType type = GetType((void*) res);
	if ((res->type != XML_ELEMENT_NODE) || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))
	    res = (xmlNodePtr) Next(res, aType);
    }
    return res;
}

void *ChromoMdlX::GetLastChild(const void *aHandle, TNodeType aType)
{
    xmlNodePtr node = (xmlNodePtr) aHandle;
    __ASSERT(node != NULL);
    xmlNodePtr res = node->last;
    if (res != NULL) {
	TNodeType type = GetType((void*) res);
	if ((res->type != XML_ELEMENT_NODE) || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))
	    res = (xmlNodePtr) Prev(res, aType);
    }
    return res;
}

void* ChromoMdlX::GetFirstTextChild(const void* aHandle)
{
    xmlNodePtr node = (xmlNodePtr) aHandle;
    __ASSERT(node != NULL);
    xmlNodePtr res = node->children;
    if (res != NULL) {
	if (res->type != XML_TEXT_NODE)
	    res = (xmlNodePtr) NextText(res);
    }
    return res;
}

void* ChromoMdlX::Root(const void* aHandle)
{
    void *res = NULL;
    xmlNodePtr parent = (xmlNodePtr) Parent(aHandle);
    if (parent == NULL) {
	res = (void*) aHandle;
    }
    else {
	res = Root(parent);
    }
    return res;
}

void* ChromoMdlX::Parent(const void* aHandle)
{
    xmlNodePtr parent = ((xmlNodePtr) aHandle)->parent;
    return parent->type == XML_ELEMENT_NODE ? parent : NULL;
}

void *ChromoMdlX::Next(const void *aHandle, TNodeType aType)
{
    __ASSERT(aHandle!= NULL);
    xmlNodePtr res = ((xmlNodePtr) aHandle)->next;
    if (res != NULL) {
	TNodeType type = GetType((void*) res);
	while ((res != NULL) && ((res->type != XML_ELEMENT_NODE) || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))) {
	    res = res->next;
	    if (res != NULL)
		type = GetType((void*) res);
	}
    }
    return res;
}

void* ChromoMdlX::NextText(const void* aHandle)
{
    __ASSERT(aHandle!= NULL);
    xmlNodePtr res = ((xmlNodePtr) aHandle)->next;
    if (res != NULL) {
	while ((res != NULL) && (res->type != XML_TEXT_NODE)) {
	    res = res->next;
	}
    }
    return res;
}

void *ChromoMdlX::Prev(const void *aHandle, TNodeType aType)
{
    __ASSERT(aHandle!= NULL);
    xmlNodePtr res = ((xmlNodePtr) aHandle)->prev;
    if (res != NULL) {
	TNodeType type = GetType((void*) res);
	while ((res != NULL) && ((res->type != XML_ELEMENT_NODE) || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown))))
	    res = res->prev;
    }
    return res;
}

char *ChromoMdlX::GetAttr(const void* aHandle, TNodeAttr aAttr)
{
    __ASSERT(aHandle != NULL);
    xmlNodePtr node = (xmlNodePtr) aHandle;
    xmlChar *attr = xmlGetProp(node, (const xmlChar *) GUri::NodeAttrName(aAttr).c_str());
    return (char *) attr;
}

TBool ChromoMdlX::AttrExists(const void* aHandle, TNodeAttr aAttr)
{
    __ASSERT(aHandle != NULL);
    TBool res = EFalse;
    xmlNodePtr node = (xmlNodePtr) aHandle;
    xmlChar *attr = xmlGetProp(node, (const xmlChar *) GUri::NodeAttrName(aAttr).c_str());
    res = (attr != NULL);
    free (attr);
    return res;
}

char* ChromoMdlX::GetContent(const void* aHandle)
{
    __ASSERT(aHandle != NULL);
    xmlNodePtr node = (xmlNodePtr) aHandle;
    xmlChar *cont = xmlNodeGetContent(node);
    return (char *) cont;
}

void  ChromoMdlX::SetContent(const void* aHandle, const string& aContent)
{
    __ASSERT(aHandle != NULL);
    xmlNodePtr node = (xmlNodePtr) aHandle;
    xmlNodeSetContent(node, (const xmlChar*) aContent.c_str());
}

int ChromoMdlX::GetAttrInt(void *aHandle, const char *aName)
{
    int res = -1;
    __ASSERT(aHandle!= NULL);
    xmlNodePtr node = (xmlNodePtr) aHandle;
    xmlChar *attr = xmlGetProp(node, (const xmlChar *) aName);
    __ASSERT(attr != NULL);
    res = atoi((const char *) attr);
    free(attr);
    return res;
}

void* ChromoMdlX::AddChild(void* aParent, TNodeType aNode)
{
    string name = GUri::NodeTypeName(aNode);
    xmlNodePtr node = xmlNewNode(NULL, (const xmlChar*) name.c_str());
    return xmlAddChild((xmlNodePtr) aParent, node);
}

void* ChromoMdlX::AddChild(void* aParent, const void* aHandle, TBool aCopy)
{
    xmlNodePtr node = aCopy ? xmlCopyNode((xmlNodePtr) aHandle, 1) : (xmlNodePtr) aHandle;
    return xmlAddChild((xmlNodePtr) aParent, node);
}

void* ChromoMdlX::AddChildDef(void* aParent, const void* aHandle, TBool aCopy)
{
    xmlNodePtr res = NULL;
    xmlNodePtr parent = (xmlNodePtr) aParent;
    xmlNodePtr node = aCopy ? xmlCopyNode((xmlNodePtr) aHandle, 1) : (xmlNodePtr) aHandle;
    xmlNodePtr pnode = FindNodeEnterigPos(parent, node);
    if (pnode != NULL) {
	res = xmlAddNextSibling(pnode, node);
    }
    else {
	if (parent->children != NULL) {
	    res = xmlAddPrevSibling(parent->children, node);
	}
	else {
	    res = xmlAddChild(parent, node);
	}
    }
    return res;
}

void* ChromoMdlX::AddNext(const void* aPrev, const void* aHandle, TBool aCopy)
{
    xmlNodePtr node = aCopy ? xmlCopyNode((xmlNodePtr) aHandle, 1) : (xmlNodePtr) aHandle;
    return xmlAddNextSibling((xmlNodePtr) aPrev, node);
}

void* ChromoMdlX::AddNext(const void* aPrev, TNodeType aNode)
{
    string name = GUri::NodeTypeName(aNode);
    xmlNodePtr node = xmlNewNode(NULL, (const xmlChar*) name.c_str());
    return xmlAddNextSibling((xmlNodePtr) aPrev, node);
}

void* ChromoMdlX::AddPrev(const void* aNext, const void* aHandle, TBool aCopy)
{
    xmlNodePtr node = aCopy ? xmlCopyNode((xmlNodePtr) aHandle, 1) : (xmlNodePtr) aHandle;
    return xmlAddPrevSibling((xmlNodePtr) aNext, node);
}

void ChromoMdlX::SetAttr(void* aNode, TNodeAttr aType, const char* aVal)
{
    string name = GUri::NodeAttrName(aType);
    if (AttrExists(aNode, aType)) {
	xmlSetProp((xmlNodePtr) aNode, (const xmlChar*) name.c_str(), (const xmlChar*) aVal);
    }
    else {
	xmlNewProp((xmlNodePtr) aNode, (const xmlChar*) name.c_str(), (const xmlChar*) aVal);
    }
}

void ChromoMdlX::SetAttr(void* aNode, TNodeAttr aType, TNodeType aVal)
{
    SetAttr(aNode, aType, GUri::NodeTypeName(aVal).c_str());
}

void ChromoMdlX::SetAttr(void* aNode, TNodeAttr aType, TNodeAttr aVal)
{
    SetAttr(aNode, aType, GUri::NodeAttrName(aVal).c_str());
}

void ChromoMdlX::RmChild(void* aParent, void* aChild, TBool aDeattachOnly)
{
    xmlNodePtr child = (xmlNodePtr) aChild;
    xmlUnlinkNode(child);
    if (!aDeattachOnly) {
	xmlFreeNode(child);
    }
}

void ChromoMdlX::Rm(void* aNode)
{
    xmlNodePtr node = (xmlNodePtr) aNode;
    xmlUnlinkNode(node);
    xmlFreeNode(node);
}

void ChromoMdlX::Dump(void* aNode, MLogRec* aLogRec)
{
    xmlBufferPtr bufp = xmlBufferCreate();	
    int	res = xmlNodeDump(bufp, iDoc, (xmlNodePtr) aNode, 0, 0);
    aLogRec->WriteFormat("%s", xmlBufferContent(bufp));
}

void ChromoMdlX::Save(const string& aFileName) const
{
    int res = xmlSaveFormatFile(aFileName.c_str(), iDoc, 4);
}

xmlElementPtr ChromoMdlX::GetElementDecl(void* aHandle)
{
    xmlNodePtr node = (xmlNodePtr) aHandle;
    struct _xmlDtd* dtd = iDoc->extSubset;
    const char* nname = (const char*) node->name;
    __ASSERT(dtd != NULL);
    xmlNodePtr res = dtd->children;
    while ((res != NULL) && !((res->type == XML_ELEMENT_DECL) && strcmp((const char*) res->name, nname) == 0))
       	res = res->next;
    return (xmlElementPtr) res;
}

xmlElementContent* ChromoMdlX::GetFirstEldeclSecCont(xmlElementPtr aElem)
{
    xmlElementContent* res = aElem->content->c1;
    return res;
}

xmlElementContent* ChromoMdlX::FindEldeclSecCont(xmlNodePtr aParent, xmlNodePtr aNode)
{
    xmlElementPtr elemdecl = GetElementDecl(aParent);
    const char* nname = (const char*) aNode->name;
    xmlElementContent* res = GetFirstEldeclSecCont(elemdecl); 
    while ((res != NULL) && !(strcmp((const char*) res->name, nname) == 0))
       	res = GetNextEldeclSecCont(res);
    return res;
}

// Gets next element within element decl content of type SEC
xmlElementContent* ChromoMdlX::GetNextEldeclSecCont(xmlElementContent* aContent)
{
    xmlElementContent* res = aContent->parent->c2->c1;
    return res;
}

xmlElementContent* ChromoMdlX::GetPrevEldeclSecCont(xmlElementContent* aContent)
{
    xmlElementContent* p1 = aContent->parent->parent;
    xmlElementContent* res = (p1 != NULL  && p1 != (xmlElementContent*) 1) ?  p1->c1 : NULL;
    return res;
}

xmlNodePtr ChromoMdlX::FindNodeEnterigPos(xmlNodePtr aParent, xmlNodePtr aNode)
{
    xmlNodePtr res = NULL;
    xmlElementContent* contc = FindEldeclSecCont(aParent, aNode);
    xmlElementContent* contp = GetPrevEldeclSecCont(contc);
    if (contp != NULL) {  
	res = (xmlNodePtr) GetLastChild(aParent, GUri::NodeType((const char*) contp->name));
	while ((res == NULL) && (contp != NULL))
	    contp = GetPrevEldeclSecCont(contp);
    }
    return res;
}


void ChromoMdlX::MoveNextTo(void* aHandle, void* aDest)
{
    xmlNodePtr src = (xmlNodePtr) aHandle;
    xmlNodePtr dest = (xmlNodePtr) aDest;
    xmlAddNextSibling(dest, src);
}

void ChromoMdlX::MovePrevTo(void* aHandle, void* aDest)
{
    xmlNodePtr src = (xmlNodePtr) aHandle;
    xmlNodePtr dest = (xmlNodePtr) aDest;
    xmlAddPrevSibling(dest, src);
}

void ChromoMdlX::MoveToEnd(void* aHandle)
{
    xmlNodePtr src = (xmlNodePtr) aHandle;
    xmlNodePtr dest = ((xmlNodePtr) aHandle)->next;
    if (dest != NULL) {
	xmlAddSibling(dest, src);
    }
}


ChromoX::ChromoX(): iRootNode(iMdl, NULL)
{
}

void ChromoX::Set(const char *aFileName)
{
    void *root = iMdl.Set(aFileName);
    iRootNode = ChromoNode(iMdl, root);
}

TBool ChromoX::Set(const string& aUri)
{
    TBool res = EFalse;
    void *root = iMdl.Set(aUri);
    if (root != NULL) {
	iRootNode = ChromoNode(iMdl, root);
	res = ETrue;
    }
    return res;
}

void ChromoX::Set(const ChromoNode& aRoot)
{
    void *root = iMdl.Set(aRoot.Handle());
    iRootNode = ChromoNode(iMdl, root);
}

ChromoX::~ChromoX()
{
}

ChromoNode& ChromoX::Root()
{
    return iRootNode;
}

const ChromoNode& ChromoX::Root() const
{
    return iRootNode;
}

void ChromoX::Reset()
{
    iMdl.Reset();
}

void ChromoX::Init(TNodeType aRootType)
{
    void *root = iMdl.Init(aRootType);
    iRootNode = ChromoNode(iMdl, root);
}

void ChromoX::Save(const string& aFileName) const
{
    iMdl.Save(aFileName);
}

ChromoNode ChromoX::CreateNode(void* aHandle)
{
    return ChromoNode(iMdl, aHandle);
}

