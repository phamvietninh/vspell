<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">
<xsl:output encoding="utf-8"/>
<xsl:template match="/">
<xsl:copy-of select=".//vne"/>
<xsl:copy-of select=".//table[@id='CContainer']"/>
</xsl:template>

</xsl:stylesheet>
