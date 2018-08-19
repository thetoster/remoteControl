#require jsmin -> pip install jsmin

import io
from jsmin import jsmin

file = io.open("src/www/index.html", mode="r", encoding="utf-8")
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
print "Original size:", orgLen, "bytes"
print "Compressed size:", newLen, "bytes"
print "Reduction ", 100 - ((newLen * 100) / orgLen), "%"

#add R"( )" to wrap html
newHtml = "R\"(" + newHtml + ")\""
file2 = io.open("src/www/index_comp.html", mode="w", encoding="utf-8")
file2.write(newHtml);
file2.close();
