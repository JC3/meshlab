#include "additionalgui.h"

CheckBoxListItemDelegate::CheckBoxListItemDelegate(QObject *parent)
: QStyledItemDelegate(parent)
{
}

void CheckBoxListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
	bool value = index.data(Qt::CheckStateRole).toBool();
	QString text = index.data(Qt::DisplayRole).toString();

	// fill style options with item data
	const QStyle *style = QApplication::style();
	QStyleOptionButton opt;
	opt.state |= value ? QStyle::State_On : QStyle::State_Off;
	opt.state |= QStyle::State_Enabled;
	opt.text = text;
	opt.rect = QRect(option.rect.x(),option.rect.y(),16,16);

	QRect textrect(option.rect.x() + 16,option.rect.y(),option.rect.width() - 16,option.rect.height());
	style->drawPrimitive(QStyle::PE_IndicatorCheckBox,&opt,painter);
	style->drawItemText(painter,textrect,Qt::AlignLeft,opt.palette,true,text);
}

CheckBoxList::CheckBoxList(const QString& defaultValue,QWidget *widget )
:QComboBox(widget),highli(0),defaultval(defaultValue),popupopen(false)
{
	view()->viewport()->installEventFilter(this);
	view()->setItemDelegate(new CheckBoxListItemDelegate(this));
	connect(this,SIGNAL(highlighted(int)),this,SLOT(currentHighlighted(int)));
}

CheckBoxList::CheckBoxList( QWidget *widget /*= 0*/ )
:QComboBox(widget),highli(0),defaultval(),popupopen(false)
{
	view()->viewport()->installEventFilter(this);
        view()->setItemDelegate(new CheckBoxListItemDelegate(this));
	connect(this,SIGNAL(highlighted(int)),this,SLOT(currentHighlighted(int)));
}

CheckBoxList::~CheckBoxList()
{
}

void CheckBoxList::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));
	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	opt.currentText = "";
	if (selectedItemsNames().empty())
		opt.currentText = defaultval;
	else
		opt.currentText = selectedItemsString(QString(" | "));
	for (int ii=0;ii<count();++ii)
	{
		Qt::CheckState v;
		if (sel.contains(itemText(ii)))
			v = Qt::Checked;
		else
			v = Qt::Unchecked;
		setItemData(ii,QVariant(v),Qt::CheckStateRole);
	}
	painter.drawComplexControl(QStyle::CC_ComboBox, opt);
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

bool CheckBoxList::eventFilter(QObject *object, QEvent * event)
{
	if ((event->type() == QEvent::MouseButtonPress) && (!popupopen))
	{
		popupopen = true;
		return true;
	}
	if((event->type() == QEvent::MouseButtonRelease) &&
		(object==view()->viewport()) && popupopen)
	{
		updateSelected(highli);
		repaint();
		popupopen = true;
		return true;
	}
	return QComboBox::eventFilter(object,event);
}

void CheckBoxList::focusOutEvent ( QFocusEvent * /*e*/ )
{
	if (popupopen)
		popupopen = false;
}

QStringList CheckBoxList::getSelected() const
{
	return sel;
}

void CheckBoxList::updateSelected(const int ind)
{
	bool checked = itemData(ind,Qt::CheckStateRole).toBool();
	QString text = itemText(highli);
	if (checked)
		sel.removeAll(text);
	else
		sel.push_back(text);
}

void CheckBoxList::insertCheckableItem( const int pos,const QString&
									   lab,const bool checked )
{
	insertItem(pos,lab);
	if (checked)
		sel.push_back(lab);
}

void CheckBoxList::insertCheckableItem(const QString& lab,const bool
									   checked )
{
	addItem(lab);
	if (checked)
		sel.push_back(lab);
}

void CheckBoxList::currentHighlighted( int high )
{
	highli = high;
}

QStringList CheckBoxList::selectedItemsNames() const
{
	return sel;
}

QString CheckBoxList::selectedItemsString(const QString& sep) const
{
	QStringList ll = selectedItemsNames();
	if (ll.isEmpty())
		return defaultval;
	return ll.join(sep);
}

void CheckBoxList::setDefaultValue( const QString& defaultValue )
{
	defaultval = defaultValue;
}

void CheckBoxList::setCurrentValue( const QStringList& st )
{
	sel = st;
	sel.removeAll(defaultval);
}

QPixmap UsefulGUIFunctions::pixmapGeneratorFromQtPrimitiveElement(const QSize& pixmapsize,const QStyle::PrimitiveElement primitive, QStyle *style,const QStyleOption& opt)
{
	QPixmap pix(pixmapsize);
	pix.fill(Qt::transparent);
	QPainter p;
	p.begin(&pix);
	style->drawPrimitive(primitive, &opt, &p);
	p.end();
	return pix;
}

QString UsefulGUIFunctions::generateUniqueDefaultName( const QString& basename,const QStringList& namelist)
{
	int max = INT_MIN;
	QString regexp(basename + "_(\\d)+");
	QStringList items = namelist.filter(QRegExp(regexp));
	for(int ii = 0;ii < items.size();++ii)
	{
		QRegExp reg("(\\d)+");
		items[ii].indexOf(reg);
		int index = reg.cap().toInt();
		if (index > max)
			max = index;
	}
	QString tmpname = basename + "_";
	if (items.size() == 0)
		tmpname += QString::number(namelist.size());
	else
		tmpname += QString::number(max + 1);
	return tmpname;
}

QString UsefulGUIFunctions::generateFunctionName(const QString& originaltext)
{
	QString newname;
	if (originaltext.isEmpty())
		return newname;
	QRegExp nonchar("\\W+");
	int index = 0;
	do 
	{
		originaltext.indexOf(nonchar,index);
		QRegExp validchar("\\w+");
		int validcharind = originaltext.indexOf(validchar,index);
		if (validcharind != -1)
		{
			QString captured = validchar.cap();
			if (captured.size() > 0)
				captured[0] = captured[0].toUpper();
			newname.push_back(captured);
		}
		index = index + validchar.cap().size() + nonchar.cap().size();
	} while (index < originaltext.size());
	if (originaltext[0].isLetter() && (newname.size() > 0))
		newname[0] = originaltext[0].toLower();
	return newname;
}

QString UsefulGUIFunctions::changeNameIfAlreadyInList( const QString& name,const QStringList& allnames )
{
	QStringList ls;
	QString tmpname = name;
	do
	{
		ls = allnames.filter(tmpname);
		if (ls.size() > 1)
			tmpname = tmpname + "_" + QString::number(ls.size() - 1);
	} while(ls.size() > 1);
	return tmpname;
}

QString UsefulGUIFunctions::generateBackupName( const QFileInfo& finfo )
{
	QDir dir = finfo.absoluteDir();
	QFileInfoList list = dir.entryInfoList(QDir::Files);
	QString oldnm = finfo.fileName();
	oldnm.replace('.',"\\.");
	QRegExp oldexp(oldnm + "\\.old(\\d+)");
	int max = 0;
	for (int ii = 0;ii < list.size();++ii)
	{
		if (list[ii].fileName().contains(oldexp))
		{
			QRegExp num("\\d+");
			list[ii].suffix().indexOf(num);
			int ver = num.cap().toInt();
			if (ver > max)
				max = ver;
		}
	}
	return QString(finfo.absolutePath() + "/" + finfo.fileName() + ".old" + QString::number(max + 1));
}

QString UsefulGUIFunctions::avoidProblemsWithHTMLTagInsideXML( const QString& text )
{
	return "<![CDATA[" + text + "]]>";
}

ExpandButtonWidget::ExpandButtonWidget( QWidget* parent )
:QWidget(parent),isExpanded(false)
{
	exp = new PrimitiveButton(QStyle::PE_IndicatorArrowDown,this);
	exp->setMaximumSize(16,16);
	QHBoxLayout *hlay = new QHBoxLayout(this);
	hlay->addWidget(exp,0,Qt::AlignHCenter);
	connect(exp,SIGNAL(clicked(bool)),this,SLOT(changeIcon()));
}

ExpandButtonWidget::~ExpandButtonWidget()
{

}

void ExpandButtonWidget::changeIcon()
{
	isExpanded = !isExpanded;
	if (isExpanded)
		exp->setPrimitiveElement(QStyle::PE_IndicatorArrowUp);
	else
		exp->setPrimitiveElement(QStyle::PE_IndicatorArrowDown);
	emit expandView(isExpanded);
}

PrimitiveButton::PrimitiveButton(const QStyle::PrimitiveElement el,QWidget* parent )
:QPushButton(parent),elem(el)
{
}

PrimitiveButton::PrimitiveButton( QWidget* parent )
:QPushButton(parent),elem(QStyle::PE_CustomBase)
{

}

PrimitiveButton::~PrimitiveButton()
{

}

void PrimitiveButton::paintEvent( QPaintEvent * event )
{
	QStylePainter painter(this);
	QStyleOptionButton option;
	option.initFrom(this);
	//painter.drawControl(QStyle::CE_PushButton,option);
	painter.drawPrimitive (elem,option);
}

void PrimitiveButton::setPrimitiveElement( const QStyle::PrimitiveElement el)
{
	elem = el;
}

TreeWidgetWithMenu::TreeWidgetWithMenu( QWidget* parent /*= NULL*/ )
:QTreeWidget(parent)
{
	menu = new QMenu(this);
	connect(menu,SIGNAL(triggered(QAction*)),this,SIGNAL(selectedAction(QAction*)));
}

TreeWidgetWithMenu::~TreeWidgetWithMenu()
{

}

void TreeWidgetWithMenu::contextMenuEvent( QContextMenuEvent * event )
{
	menu->popup(event->globalPos());
}

void TreeWidgetWithMenu::insertInMenu(const QString& st,const QVariant& data)
{
	QAction* act = menu->addAction(st);
	act->setData(data);
}

MLScriptEditor::MLScriptEditor( QWidget* par /*= NULL*/ )
:QPlainTextEdit(par),regexps(),synt(NULL),synhigh(NULL),comp(NULL)
{
	QTextDocument* mydoc = new QTextDocument(this);
	QPlainTextDocumentLayout* ld = new QPlainTextDocumentLayout(mydoc);
	mydoc->setDocumentLayout(ld);
	setDocument(mydoc);

	narea = new MLNumberArea(this);
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	//connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateCursorPos(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

MLScriptEditor::~MLScriptEditor()
{

}

void MLScriptEditor::lineNumberAreaPaintEvent( QPaintEvent *event,const QColor& col)
{
	QPainter painter(narea);
	painter.fillRect(event->rect(),col);
	QTextBlock block = firstVisibleBlock();
	int indent = block.blockFormat().indent();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();
	while (block.isValid() && top <= event->rect().bottom()) 
	{
		if (block.isVisible() && bottom >= event->rect().top()) 
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, narea->width(), fontMetrics().height(),Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

int MLScriptEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void MLScriptEditor::resizeEvent( QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	narea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void MLScriptEditor::updateLineNumberAreaWidth( int newBlockCount)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void MLScriptEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void MLScriptEditor::updateLineNumberArea( const QRect & r, int dy)
{
	if (dy)
		narea->scroll(0, dy);
	else
		narea->update(0, r.y(), narea->width(), r.height());

	if (r.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

QString MLScriptEditor::currentLine() const
{
	QTextCursor cur = textCursor();
	cur.select(QTextCursor::LineUnderCursor);
	return cur.selectedText();
}

void MLScriptEditor::keyPressEvent( QKeyEvent * e )
{
	switch(e->key())
	{
		case (Qt::Key_Return):
		case (Qt::Key_Enter):
		{
			if (comp->popup()->isHidden())
			{
				QTextBlock b = textCursor().block();
				QRegExp tab("(\\t)+\\w");
				bool tabfound = (b.text().indexOf(tab) == 0);
				textCursor().insertText("\n");
				if (tabfound)
				{
					QString cap = tab.cap();
					int tabcount = cap.lastIndexOf(QRegExp("\\t")) + 1;
					QString tabst;
					for(int ii = 0;ii < tabcount;++ii)
						tabst += '\t';
					textCursor().insertText(tabst);
				}
			}
			else
			{
				insertSuggestedWord(comp->currentCompletion());
				comp->popup()->hide();
			}
			return;
		}
		case (Qt::Key_Tab):
		{
			if (!comp->popup()->isHidden())
			{
				insertSuggestedWord(comp->currentCompletion());
				comp->popup()->hide();
				return;
			}
			break;
		}
	}
	QPlainTextEdit::keyPressEvent(e);
	//!(e->text().isEmpty) is meaningful: you need it when (only) a modifier (SHIFT/CTRL) has been pressed in order to avoid the autocompleter to be visualized
	if (!(e->text().isEmpty()) && (e->text().indexOf(synt->worddelimiter) == -1))
		showAutoComplete(e);
}

//void MLScriptEditor::setSyntaxHighlighter( MLSyntaxHighlighter* high )
//{
//	slh = high;
//	if (slh != NULL)
//	{
//		slh->setDocument(document());
//		connect(&slh->comp,SIGNAL(activated(const QString &)),this,SLOT(insertSuggestedWord( const QString &)));
//	}
//}

void MLScriptEditor::showAutoComplete( QKeyEvent * e )
{	
	QString w = wordUnderTextCursor();
	comp->setCompletionPrefix(w);
	comp->popup()->setModel(comp->completionModel());
	QRect rect = cursorRect();
	rect.setWidth(comp->popup()->sizeHintForColumn(0) + comp->popup()->verticalScrollBar()->sizeHint().width());
	comp->complete(rect);
}

void MLScriptEditor::insertSuggestedWord( const QString& str )
{
	QTextCursor tc = textCursor();
	int extra = str.length() - comp->completionPrefix().length();
	tc.insertText(str.right(extra));
	setTextCursor(tc);
}

QString MLScriptEditor::lastInsertedWord() const
{
	QString cur = currentLine();
	QStringList ls = cur.split(synt->worddelimiter,QString::SkipEmptyParts);
	if (ls.size() > 0)
		return ls[ls.size() - 1];
	return QString();
}

void MLScriptEditor::setScriptLanguage( MLScriptLanguage* syntax )
{
	if (syntax != NULL)
	{
		delete synt;
		synt = syntax;
		delete synhigh;
		synhigh = new MLSyntaxHighlighter(*synt,this);
		synhigh->setDocument(document());
		delete comp;
		comp = new MLAutoCompleter(*synt,this);
		comp->setCaseSensitivity(Qt::CaseSensitive);
		comp->setWidget(this);
		comp->setCompletionMode(QCompleter::PopupCompletion);
		comp->setModel(synt->functionsLibrary());
		connect(comp,SIGNAL(activated(const QString &)),this,SLOT(insertSuggestedWord( const QString &)));
		connect(comp,SIGNAL(highlighted(const QModelIndex&)),comp,SLOT(changeCurrent(const QModelIndex&)));

	}
}

QString MLScriptEditor::wordUnderTextCursor() const
{
	QTextCursor tc = this->textCursor();
	int endpos = tc.position();
	tc.select(QTextCursor::LineUnderCursor);
	QString line = tc.selectedText();
	line = line.left(endpos);
	QRegExp id = synt->matchIdentifier();
	int index = 0; 
	while ((index >= 0) && (index < line.size()))
	{
		int tmp = line.indexOf(id,index);
		index = tmp + id.matchedLength();
	}

	if (index >= 0)
		return id.cap();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

MLNumberArea::MLNumberArea( MLScriptEditor* editor ) : QWidget(editor)
{
	mledit = editor;
}

QSize MLNumberArea::sizeHint() const
{
	return QSize(mledit->lineNumberAreaWidth(), 0);
}

void MLNumberArea::paintEvent(QPaintEvent* e)
{
	mledit->lineNumberAreaPaintEvent(e,UsefulGUIFunctions::editorMagicColor());
}

MLSyntaxHighlighter::MLSyntaxHighlighter(const MLScriptLanguage& synt, QWidget* parent)
:QSyntaxHighlighter(parent),highlightingRules(),syntax(synt)
{
	HighlightingRule pvar;
	pvar.format.setForeground(Qt::red);
	HighlightingRule res;
	res.format.setForeground(Qt::darkBlue);
	res.format.setFontWeight(QFont::Bold);
	foreach(QString word,synt.reserved)
	{
		res.pattern = QRegExp(addIDBoundary(word));
		highlightingRules << res;
	}
}

void MLSyntaxHighlighter::highlightBlock( const QString& text )
{
	
	/*foreach (const HighlightingRule &rule, highlightingRules) 
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) 
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}*/
	HighlightingRule rule;
	rule.format.setForeground(Qt::darkBlue);
	rule.format.setFontWeight(QFont::Bold);
	QRegExp keyword = syntax.matchOnlyReservedWords();
	int index = keyword.indexIn(text);
	while(index >=0)
	{
		
		int length = keyword.matchedLength();
		setFormat(index, length, rule.format);
		index = keyword.indexIn(text, index + length);
	}
	QRegExp nokeyword = syntax.matchIdentifiersButNotReservedWords();
	QString tmp = nokeyword.pattern();
	SyntaxTreeNode* root = syntax.functionsLibrary()->getItem(QModelIndex());
	index = 0;
	//QTextCharFormat form;
	//form.setForeground(Qt::red);
	while(index >= 0)
	{
		index = nokeyword.indexIn(text,index);
		if (index >= 0)
		{
			bool matchedchar = colorTextIfInsideTree(nokeyword.cap(),root,index);
			index = index + nokeyword.matchedLength();
		}
	}
	setCurrentBlockState(0);
}

QString MLSyntaxHighlighter::addIDBoundary( const QString& st )
{
	return "\\b" + st + "\\b";
}

bool MLSyntaxHighlighter::colorTextIfInsideTree(const QString& text,SyntaxTreeNode* node,int start)
{
	QTextCharFormat form;
	form.setForeground(Qt::red);
	QRegExp exp;
	//int nextmatchstart = start;
	if (node != NULL)
	{
		//it's the root. The root is not meaningful
		if (node->parent() == NULL)
		{
			int ii = 0;
			while(ii < node->childCount())
			{
				bool found = colorTextIfInsideTree(text,node->child(ii),start);
				if (found)
					return true;
				++ii;
			}	
		}
		QString nodevalue =  addIDBoundary(node->data(0).toString());
		exp.setPattern( nodevalue + "(\\s*\\" + node->data(2).toString() + "\\s*)?");
		
		int index = exp.indexIn(text);
		if (index < 0)
		{
			//setCurrentBlockState(0);
			return false;
		}
		setFormat(start + index, exp.matchedLength(), form);
		if (text.size() == exp.matchedLength())
			return true;
		int ii = 0;
		while(ii < node->childCount())
		{
			bool found = colorTextIfInsideTree(text.right(text.size() - (exp.matchedLength())),node->child(ii),start + exp.matchedLength());
			if (found)
				return true;
			++ii;
		}
		return false;
	}
	return false;
}

MLAutoCompleter::MLAutoCompleter( const MLScriptLanguage& synt,QWidget* parent )
:QCompleter(parent),syntax(synt)
{
	setCompletionRole(Qt::DisplayRole);
	MLAutoCompleterPopUp* pop = new MLAutoCompleterPopUp(parent);
	setPopup(pop);
}

QStringList MLAutoCompleter::splitPath( const QString &path ) const
{
	QString tmp = path;
	QString parst = "\\s*" + syntax.openpar.pattern() + "." + syntax.closepar.pattern();
	QRegExp par(parst);
	tmp.remove(par);
	QStringList res = path.split(syntax.wordsjoiner);
	return res;
}

QString MLAutoCompleter::pathFromIndex( const QModelIndex &index ) const
{
	QString completename;
	for (QModelIndex i = index; i.isValid(); i = i.parent()) 
	{
		QString tmp = model()->data(i, completionRole()).toString();
		if (i != index)
		{
			QModelIndex sepindex = i.sibling(i.row(),2);
			if (sepindex.isValid())
				tmp = model()->data(sepindex).toString() + tmp;
		}
		completename = tmp + completename;

	}
	return completename;
}

void MLAutoCompleter::changeCurrent( const QModelIndex& ind )
{
	setCurrentRow(ind.row());
}


MLAutoCompleterPopUp::MLAutoCompleterPopUp( QWidget* parent )
:QListView(parent)
{
}

MLAutoCompleterPopUp::~MLAutoCompleterPopUp()
{
}

bool MLAutoCompleterPopUp::event( QEvent *event )
{
	if (event->type() == QEvent::ToolTip) 
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		QModelIndex indexid = indexAt(helpEvent->pos());
		QModelIndex indexsign = indexid.sibling(indexid.row(),1);
	
		if (indexsign.isValid()) 
			QToolTip::showText(helpEvent->globalPos(), indexsign.data().toString());
		else 
		{
			QToolTip::hideText();
			event->ignore();
		}
		return true;
	}
	return QWidget::event(event);
}