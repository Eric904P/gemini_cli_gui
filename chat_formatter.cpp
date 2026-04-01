/**
 * @file chat_formatter.cpp
 * @brief Implementation of the markdown-to-HTML parser.
 *
 * Uses QRegularExpression to detect code boundaries, builds HTML 
 * layout tables, and strictly isolates syntax highlighting to prevent 
 * conversational text from bleeding into the IDE themes.
 */

#include "chat_formatter.h"

#include <QStringList>
#include <QRegularExpression>

QString ChatFormatter::formatMarkdownToHtml(const QString& markdown) {
    QString html;
    QStringList lines = markdown.split('\n');
    bool inCodeBlock = false;

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();

        // detect markdown code block boundaries
        if (trimmed.startsWith("```")) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                QString lang = trimmed.mid(3).trimmed();
                if (lang.isEmpty()) lang = "CODE";

                // build a web-like ui box with a header bar
                html += QString("<br><table width=\"100%\" style=\"background-color: #F8F9FA; border: 1px solid #E5E7EB;\">"
                                "<tr><td style=\"background-color: #E5E7EB; padding: 4px 8px; font-family: sans-serif; font-size: 11px; color: #374151;\"><b>%1</b></td></tr>"
                                "<tr><td style=\"padding: 8px; font-family: 'Courier New', monospace; font-size: 13px; color: #1F2937;\">")
                                .arg(lang.toUpper());
            } else {
                inCodeBlock = false;
                // close the web ui box
                html += "</td></tr></table><br>";
            }
        } else {
            // escape html so <iostream> doesn't disappear
            QString escaped = line.toHtmlEscaped();

            if (inCodeBlock) {
                // --- inline syntax highlighting (only applies inside code blocks!) ---
                escaped.replace(QRegularExpression("(&quot;.*?&quot;|'.*?')"), "<span style=\"color: #A31515;\">\\1</span>"); // strings
                escaped.replace(QRegularExpression("(//.*|#.*)"), "<span style=\"color: #008000;\">\\1</span>"); // comments
                escaped.replace(QRegularExpression("\\b(def|class|return|if|for|while|import|from|int|void|bool|auto|const|let|var|function|namespace|public|private)\\b"), "<span style=\"color: #0000FF; font-weight: bold;\">\\1</span>"); // keywords
                escaped.replace(QRegularExpression("\\b([A-Za-z0-9_]+)(?=\\()"), "<span style=\"color: #795E26;\">\\1</span>"); // functions

                // preserve python/c++ indentation spaces in html
                escaped.replace("  ", "&nbsp;&nbsp;"); 
                html += escaped + "<br>";
            } else {
                // --- conversation text formatting ---
                // format **bold** text
                escaped.replace(QRegularExpression("\\*\\*(.*?)\\*\\*"), "<b>\\1</b>"); 
                // format `inline code`
                escaped.replace(QRegularExpression("`(.*?)`"), "<span style=\"background-color: #F3F4F6; font-family: monospace; padding: 2px 4px; border-radius: 3px;\">\\1</span>"); 
                
                html += escaped + "<br>";
            }
        }
    }

    // failsafe in case the llm forgets to close the code block
    if (inCodeBlock) {
        html += "</td></tr></table><br>";
    }

    return html;
}