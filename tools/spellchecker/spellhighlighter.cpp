#include "spellhighlighter.h"

#include "common.h"
#include "spellchecker.h"

SpellHighlighter::SpellHighlighter(QTextDocument* d) : QSyntaxHighlighter(d)
{
}

void SpellHighlighter::highlightBlock(const QString& text)
{
    // Underline
    QTextCharFormat tcf;
    tcf.setUnderlineColor(QColor(255,0,0));
    if(qVersionInt() >= 0x040400 && qVersionInt() < 0x040402) {
        tcf.setUnderlineStyle(QTextCharFormat::DotLine);
    }
    else {
        tcf.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    }

    // Match words (minimally)
    QRegExp expression("\\b\\w+\\b");
    QRegExp digit("\\d+");

    // Iterate through all words
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        QString word = expression.cap();
        if (!digit.exactMatch(word) && !SpellChecker::instance()->isCorrect(word))
            setFormat(index, length, tcf);
        index = text.indexOf(expression, index + length);
    }
}
