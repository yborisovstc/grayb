

#include "mlog.h"
#include "chromox.h"

const char* KChromoSystemId = "/usr/share/grayb/conf/objspec.dtd";

ChromoMdlX::ChromoMdlX(): Base(), iDoc(NULL), iDocOwned(EFalse)
{
};

ChromoMdlX::~ChromoMdlX()
{
    Reset(); 
}

THandle ChromoMdlX::Init(TNodeType aRootType)
{
    iDocOwned = ETrue;
    iDoc = xmlNewDoc((const xmlChar*) "1.0");
    string sroottype = TMut::NodeTypeName(aRootType);
    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar *) sroottype.c_str());
    xmlDocSetRootElement(iDoc, root);
    return THandle(root);
}

void ChromoMdlX::Reset()
{
    if (iDocOwned) {
	xmlFreeDoc(iDoc);
    }
    iDoc = NULL;
}

THandle ChromoMdlX::Find(const THandle& aHandle, const string& aUri)
{
    GUri desuri(aUri);
    const vector<GUri::TElem>& elems = desuri.Elems();
    xmlNodePtr fres = NULL;
    xmlNodePtr res = aHandle.Data(res);
    for (vector<GUri::TElem>::const_iterator it = desuri.Elems().begin(); it != desuri.Elems().end() && res != NULL; it++) {
	GUri::TElem elem = *it;
	while (res != NULL) {
	    if (res->type == XML_ELEMENT_NODE) {
		TNodeType type = GetType((void*) res);
		char *name = (char*) xmlGetProp(res, (const xmlChar *) TMut::NodeAttrName(ENa_Id).c_str());
		char *parent = (char*) xmlGetProp(res, (const xmlChar *) TMut::NodeAttrName(ENa_Parent).c_str());
		string eparent;
		char rel = GUri::GetExtFirstPart(elem.ext(), eparent);
		if (type == ENt_Node && rel == GUri::KParentSep && (eparent == GUri::KTypeAny || eparent.compare(parent) == 0) && elem.name().compare(name) == 0) {
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

THandle ChromoMdlX::SetFromFile(const string& aFileName)
{
    xmlNode *sEnv = NULL; // Node of environment element
    // Read and parse the CAE spec file
    Reset();
    iDoc = xmlReadFile(aFileName.c_str(), NULL, XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID);
    __ASSERT(iDoc != NULL);
    // Get the node 
    THandle sRoot = GetFirstChild((void *) iDoc, ENt_Node);
    iDocOwned = EFalse;
    return sRoot;
}

THandle ChromoMdlX::SetFromSpec(const string& aSpec)
{
    xmlNode *sEnv = NULL; // Node of environment element
    // Read and parse the CAE spec file
    Reset();
    iDoc = xmlReadMemory(aSpec.c_str(), aSpec.size(), "noname.xml", NULL, XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID);
    __ASSERT(iDoc != NULL);
    // Get any node, not necessary ENt_Node, ref ds_mut_osmlc_impl
    THandle sRoot = GetFirstChild((void *) iDoc);
    iDocOwned = EFalse;
    return sRoot;
}

THandle ChromoMdlX::Set(const string& aUri)
{
    THandle res;
    xmlNode *sEnv = NULL; // Node of environment element
    // Read and parse the CAE spec file
    Reset();
    string path;
    ChromoUtils::GetPath(aUri, path);
    iDoc = xmlReadFile(path.c_str(), NULL, XML_PARSE_DTDLOAD | XML_PARSE_DTDVALID);
    __ASSERT(iDoc != NULL);
    // Get the node 
    string desuri;
    ChromoUtils::GetFrag(aUri, desuri);
    THandle sRoot = GetFirstChild((void *) iDoc, ENt_Node);
    if (!desuri.empty()) {
	// Transform into rel uri if abs
	// TODO [YB] - to consider if it would be better to update Find to accept the abs uri
	GUri desurib = GUri(desuri);
	GUri::const_elem_iter it = desurib.Elems().begin();
	if (it->name().empty()) {
	    it++;
	}
	desuri = desurib.toString(it);
	res = Find(sRoot, desuri); 
    }
    else {
	res = sRoot;
    }
    iDocOwned = EFalse;
    return res;
}

THandle ChromoMdlX::Set(const THandle& aHandle)
{
    if (iDoc == NULL) {
	iDocOwned = ETrue;
	iDoc = xmlNewDoc((const xmlChar*) "1.0");
	xmlDtdPtr dtd = xmlParseDTD(NULL, (const xmlChar*) KChromoSystemId );
	iDoc->extSubset = dtd;
    }
    xmlNodePtr node = xmlDocCopyNode(aHandle.Data<xmlNodePtr>(), iDoc, 1);
    xmlDocSetRootElement(iDoc, node);
    return THandle(node);
}

TNodeType ChromoMdlX::GetType(const THandle& aHandle)
{
    TNodeType res = ENt_Unknown;
    xmlNodePtr node = aHandle.Data(node);
    if (node->type == XML_COMMENT_NODE) {
	res = ENt_Note;
    } else if (node->type == XML_ELEMENT_NODE && node->name != NULL) {
	const char* type_name = (const char*) node->name;
	res = TMut::NodeType(type_name);
    }
    return res;
}

THandle ChromoMdlX::GetFirstChild(const THandle& aHandle, TNodeType aType)
{
    xmlNodePtr node = aHandle.Data(node);
    __ASSERT(node != NULL);
    THandle res(node->children);
    if (res != NULL) {
	TNodeType type = GetType(res);
	xmlNodePtr resp = res.Data<xmlNodePtr>();
	while ((resp != NULL) && ((resp->type != XML_ELEMENT_NODE && resp->type != XML_COMMENT_NODE)
		    || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))) {
	    res = Next(res, aType);
	    resp = res.Data<xmlNodePtr>();
	    if (res != NULL)
		type = GetType(res);
	}
    }
    return THandle(res);
}

THandle ChromoMdlX::GetLastChild(const THandle& aHandle, TNodeType aType)
{
    xmlNodePtr node = aHandle.Data(node);
    __ASSERT(node != NULL);
    THandle res(node->last);
    if (res != NULL) {
	TNodeType type = GetType(res);
	xmlNodePtr resp = res.Data<xmlNodePtr>();
	while ((resp != NULL) && ((resp->type != XML_ELEMENT_NODE && resp->type != XML_COMMENT_NODE)
		    || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))) {
	    res = Prev(res, aType);
	    resp = res.Data<xmlNodePtr>();
	    if (res != NULL)
		type = GetType(res);
	}
    }
    return res;
}

THandle ChromoMdlX::Root(const THandle& aHandle)
{
    THandle res;
    THandle parent = Parent(aHandle);
    if (parent == NULL) {
	res = aHandle;
    }
    else {
	res = Root(parent);
    }
    return res;
}

THandle ChromoMdlX::Parent(const THandle& aHandle)
{
    xmlNodePtr node = aHandle.Data<xmlNodePtr>();
    xmlNodePtr parent = node->parent;
    return (parent != NULL && parent->type == XML_ELEMENT_NODE) ? THandle(parent) : THandle();
}

THandle ChromoMdlX::Next(const THandle& aHandle, TNodeType aType)
{
    __ASSERT(aHandle!= NULL);
    xmlNodePtr res = aHandle.Data<xmlNodePtr>()->next;
    if (res != NULL) {
	TNodeType type = GetType(res);
	while ((res != NULL) && ((res->type != XML_ELEMENT_NODE && res->type != XML_COMMENT_NODE)
		    || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))) {
	    res = res->next;
	    if (res != NULL)
		type = GetType(res);
	}
    }
    return res;
}

THandle ChromoMdlX::Prev(const THandle& aHandle, TNodeType aType)
{
    __ASSERT(aHandle!= THandle());
    xmlNodePtr res = aHandle.Data(res)->prev;
    if (res != NULL) {
	TNodeType type = GetType(res);
	while ((res != NULL) && ((res->type != XML_ELEMENT_NODE && res->type != XML_COMMENT_NODE)
		    || ((aType != ENt_Unknown) ? (type != aType) : (type == ENt_Unknown)))) {
	    res = res->prev;
	    type = GetType(res);
	}
    }
    return res;
}

string ChromoMdlX::GetAttr(const THandle& aHandle, TNodeAttr aAttr) const
{
    __ASSERT(aHandle != THandle());
    xmlNodePtr node = aHandle.Data(node);
    xmlChar* attr = NULL;
    if (node->type == XML_COMMENT_NODE) {
	if (aAttr == ENa_MutVal) {
	    attr = xmlNodeGetContent(node);
	}
    } else {
	attr = xmlGetProp(node, (const xmlChar *) TMut::NodeAttrName(aAttr).c_str());
    }
    string res;
    if (attr != NULL)
	res.assign((char*) attr);
    free(attr); 
    return res;
}

void ChromoMdlX::SetAttr(const THandle& aHandle, TNodeAttr aAttr, TInt aVal)
{
    stringstream sval;
    sval << aVal;
    SetAttr(aHandle, aAttr, sval.str().c_str());
}

TBool ChromoMdlX::AttrExists(const THandle& aHandle, TNodeAttr aAttr) const
{
    __ASSERT(aHandle != THandle());
    TBool res = EFalse;
    xmlNodePtr node = aHandle.Data(node);
    xmlChar *attr = NULL;
    if (node->type == XML_COMMENT_NODE) {
	if (aAttr == ENa_MutVal) {
	    attr = xmlNodeGetContent(node);
	}
    } else {
	attr = xmlGetProp(node, (const xmlChar *) TMut::NodeAttrName(aAttr).c_str());
    }
    res = (attr != NULL);
    free (attr);
    return res;
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

THandle ChromoMdlX::AddChild(const THandle& aParent, TNodeType aNode)
{
    string name = TMut::NodeTypeName(aNode);
    xmlNodePtr node = xmlNewNode(NULL, (const xmlChar*) name.c_str());
    return xmlAddChild(aParent.Data<xmlNodePtr>(), node);
}

THandle ChromoMdlX::AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy, TBool aRecursively)
{
    xmlNodePtr node = aCopy ? xmlCopyNode(aHandle.Data<xmlNodePtr>(), aRecursively ? 1:2) : aHandle.Data<xmlNodePtr>();
    return xmlAddChild(aParent.Data<xmlNodePtr>(), node);
}

THandle ChromoMdlX::AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy)
{
    xmlNodePtr handle = aHandle.Data(handle);
    xmlNodePtr node = aCopy ? xmlCopyNode(handle, 1) : handle;
    return xmlAddNextSibling(aPrev.Data<xmlNodePtr>(), node);
}

THandle ChromoMdlX::AddNext(const THandle& aPrev, TNodeType aNode)
{
    string name = TMut::NodeTypeName(aNode);
    xmlNodePtr node = xmlNewNode(NULL, (const xmlChar*) name.c_str());
    return xmlAddNextSibling(aPrev.Data<xmlNodePtr>(), node);
}

THandle ChromoMdlX::AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy)
{
    xmlNodePtr handle = aHandle.Data(handle);
    xmlNodePtr node = aCopy ? xmlCopyNode(handle, 1) : handle;
    return xmlAddPrevSibling(aNext.Data<xmlNodePtr>(), node);
}

void ChromoMdlX::SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal)
{
    string name = TMut::NodeAttrName(aType);
    xmlNodePtr node = aNode.Data(node);
    if (AttrExists(aNode, aType)) {
	xmlSetProp(node, (const xmlChar*) name.c_str(), (const xmlChar*) aVal.c_str());
    } else {
	xmlNewProp(node, (const xmlChar*) name.c_str(), (const xmlChar*) aVal.c_str());
    }
}

void ChromoMdlX::RmAttr(const THandle& aNode, TNodeAttr aType)
{
    xmlNodePtr node = aNode.Data(node);
    xmlAttrPtr attr = xmlHasProp(node, (const xmlChar *) TMut::NodeAttrName(aType).c_str());
    if (attr != NULL) {
	xmlRemoveProp(attr);
    }
}

void ChromoMdlX::RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly)
{
    xmlNodePtr child = aChild.Data(child);
    xmlUnlinkNode(child);
    if (!aDeattachOnly) {
	xmlFreeNode(child);
    }
}

void ChromoMdlX::Rm(const THandle& aNode)
{
    xmlNodePtr node = aNode.Data(node);
    xmlUnlinkNode(node);
    xmlFreeNode(node);
}

void ChromoMdlX::Dump(const THandle& aNode)
{
}

void ChromoMdlX::DumpBackTree(const THandle& aNode)
{
}

void ChromoMdlX::DumpToLog(const THandle& aNode, MLogRec* aLogRec)
{
    xmlBufferPtr bufp = xmlBufferCreate();	
    xmlNodePtr node = aNode.Data(node);
    int	res = xmlNodeDump(bufp, iDoc, node, 0, 0);
    aLogRec->WriteFormat("%s", xmlBufferContent(bufp));
}

TBool ChromoMdlX::ToString(const THandle& aNode, string& aString) const
{
    xmlBufferPtr bufp = xmlBufferCreate();	
    xmlNodePtr node = aNode.Data(node);
    int	res = xmlNodeDump(bufp, iDoc, node, 0, 0);
    aString.assign((const char*)(xmlBufferContent(bufp)));
    return (res != 0);
}

void ChromoMdlX::Save(const string& aFileName) const
{
    int res = xmlSaveFormatFile(aFileName.c_str(), iDoc, 4);
}

TInt ChromoMdlX::GetOrder(const THandle& aHandle, TBool aTree) const
{
    TInt res = 0;
    /* TODO to remove support of chromo order
       TNodeAttr attr = aTree ? ENa_TOrder : ENa_Order;
       if (AttrExists(aHandle, attr))
       GetAttr(aHandle, attr, res);
       */
    return res;
}

void ChromoMdlX::DeOrder(const THandle& aHandle)
{
    xmlNodePtr node = aHandle.Data(node);
    __ASSERT(node != NULL);
    RmAttr(aHandle, ENa_Order);
    xmlNodePtr child = node->children;
    while (child != NULL) {
	DeOrder(child);
	child = child->next;
    };
}

TInt ChromoMdlX::GetLineId(const THandle& aHandle) const
{
    xmlNodePtr src = aHandle.Data(src);
    return src->line;
}


ChromoX::ChromoX(): iRootNode(iMdl, NULL)
{
}

ChromoX::ChromoX(const ChromoX& aSrc)
{
    Set(aSrc.Root());
}

void ChromoX::SetFromFile(const string& aFileName)
{
    THandle root = iMdl.SetFromFile(aFileName);
    iRootNode = ChromoNode(iMdl, root);
}

TBool ChromoX::Set(const string& aUri)
{
    TBool res = EFalse;
    THandle root = iMdl.Set(aUri);
    if (root != THandle()) {
	iRootNode = ChromoNode(iMdl, root);
	res = ETrue;
    }
    return res;
}

TBool ChromoX::SetFromSpec(const string& aSpec)
{
    TBool res = EFalse;
    THandle root = iMdl.SetFromSpec(aSpec);
    if (root != THandle()) {
	iRootNode = ChromoNode(iMdl, root);
	res = ETrue;
    }
    return res;

}

void ChromoX::Set(const ChromoNode& aRoot)
{
    THandle root = iMdl.Set(aRoot.Handle());
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
    THandle root = iMdl.Init(aRootType);
    iRootNode = ChromoNode(iMdl, root);
}

void ChromoX::Save(const string& aFileName) const
{
    // We cannot simply save the doc (iMdl.iDoc) because it will save not only root node but
    // also adjacent nodes. So we need to create new model and add to doc only one separated root
    ChromoMdlX mdl;
    mdl.Set(iRootNode.Handle());
    mdl.Save(aFileName);
    //iMdl.Save(aFileName);
}

ChromoNode ChromoX::CreateNode(const THandle& aHandle)
{
    return ChromoNode(iMdl, aHandle);
}

void ChromoX::ReduceToSelection(const ChromoNode& aSelNode)
{
    ChromoNode sel = aSelNode;
    ChromoNode prnt = *sel.Parent();
    while (prnt != ChromoNode()) {
	prnt.ReduceToSelection(sel);
	sel = prnt;
	prnt = *sel.Parent();
    }
}

TBool ChromoX::GetSpec(string& aSpec)
{
    return iMdl.ToString(iRootNode.Handle(), aSpec);
}

void ChromoX::Convert(const MChromo& aSrc)
{
    __ASSERT(false);
}
