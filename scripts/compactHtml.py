#require jsmin -> pip install jsmin

import io
from jsmin import jsmin

def compressHtml(inFileName, outFileName):
    file = io.open("src/www/" + inFileName, mode="r", encoding="utf-8")
    html = file.read();
    file.close();
    orgLen = len(html)

    try:
        newHtml = jsmin(html)
    except Exception,e:
        print str(e)
        newHtml = html

    newLen = len(newHtml)

    print "====== COMPRESS HTML ======"
    print "File:", inFileName, "->", outFileName
    print "Original size:", orgLen, "bytes"
    print "Compressed size:", newLen, "bytes"
    print "Reduction ", 100 - ((newLen * 100) / orgLen), "%"

    #add R"( )" to wrap html
    newHtml = "R\"(" + newHtml + ")\""
    file2 = io.open("src/www/" + outFileName, mode="w", encoding="utf-8")
    file2.write(newHtml);
    file2.close();

compressHtml("index.html", "index_comp.html")
compressHtml("netSetup.html", "netSetup_comp.html")