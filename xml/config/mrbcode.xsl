<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

    <xsl:template match="/">
        <html>
            <head/>
            <body style="font-family:Courier; font-size:13px;" bgcolor="#fffacd">
                <xsl:apply-templates/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="file">
        <h3 align="center">
            <xsl:value-of select="fileHdr/xname"/>
        </h3>
        <hr/>
        <xsl:apply-templates/>
        <div style="background-color: #ffa500;">[end of file:&#160;<xsl:apply-templates
                select="fileHdr/gname"/>]</div>
    </xsl:template>

    <xsl:template match="fileHdr">
        <br/>
        <table align="center" width="98%" bgcolor="#ffa500">
            <tr>
                <td colspan="2" bgcolor="#cdc9a5">[file header]</td>
                <td colspan="2" bgcolor="#cdc9a5">
                    <xsl:apply-templates select="purp"/>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Generated code</td>
                <td width="85%">
                    <xsl:apply-templates select="gname"/>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Author</td>
                <td width="85%">
                    <xsl:apply-templates select="author"/>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Mail</td>
                <td width="85%">
                    <a>
                        <xsl:attribute name="href">mailto:<xsl:value-of select="mail"/></xsl:attribute>
                        <xsl:apply-templates select="mail"/>
                    </a>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">URL</td>
                <td width="85%">
                    <a>
                        <xsl:attribute name="href">
                            <xsl:value-of select="url"/>
                        </xsl:attribute>
                        <xsl:apply-templates select="url"/>
                    </a>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Revision</td>
                <td width="85%">
                    <xsl:value-of select="version"/>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Date</td>
                <td width="85%">
                    <xsl:value-of select="date"/>
                </td>
            </tr>
        </table>
        <xsl:apply-templates select="rootClassList"/>
        <xsl:apply-templates select="mrbClassList"/>
        <hr/>
    </xsl:template>

    <xsl:template match="method">
        <br/>
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <xsl:apply-templates/>
            <table align="center" width="98%">
                <tr>
                    <td>
                        <b>}</b>
                    </td>
                </tr>
                <tr>
                    <td bgcolor="#ffa500">[end of method:&#160;<xsl:value-of
                            select="methodHdr/cname"/>::<xsl:value-of select="methodHdr/mname"
                    />()]</td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <xsl:template match="methodHdr">
        <br/>
        <table align="center" width="98%" bgcolor="#ffa500">
            <tr>
                <td colspan="2" bgcolor="#cdc9a5">[C++ method]</td>
                <td colspan="2" bgcolor="#cdc9a5">
                    <xsl:value-of select="purp"/>
                </td>
            </tr>
            <xsl:if test="../tag != &quot;&quot;">
                <tr>
                    <td width="1%"/>
                    <td width="13%">Tag</td>
                    <td width="85%">
                        <b>
                            <xsl:value-of select="../tag"/>
                        </b>
                    </td>
                </tr>
            </xsl:if>
            <tr>
                <td width="1%"/>
                <td width="13%">Name</td>
                <td width="85%"><xsl:apply-templates select="mname"/>()</td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Class</td>
                <td width="85%">
                    <xsl:apply-templates select="cname"/>
                </td>
            </tr>
        </table>
        <xsl:if test="argList != &quot;&quot;">
            <xsl:apply-templates select="argList"/>
        </xsl:if>
        <xsl:apply-templates select="returnVal"/>
        <xsl:if test="descr != &quot;&quot;">
            <table align="center" width="98%" bgcolor="#ffa500">
                <tr>
                    <td width="1%"/>
                    <td width="13%">Description</td>
                    <td width="85%">
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </table>
        </xsl:if>
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td/>
                    <td/>
                    <td>
                        <b>
                            <xsl:apply-templates select="returnVal/arg/atype"
                                />&#160;<xsl:value-of select="cname"/>::<xsl:value-of
                                select="mname"/>
                            <xsl:for-each select="argList/arg">
                                <xsl:choose>
                                    <xsl:when test="position() = 1">(</xsl:when>
                                </xsl:choose>
                                <xsl:apply-templates select="atype"/>&#160;<xsl:value-of
                                    select="aname"/>
                                <xsl:choose>
                                    <xsl:when test="position() = last()">)&#160;{</xsl:when>
                                    <xsl:otherwise>,</xsl:otherwise>
                                </xsl:choose>
                            </xsl:for-each>
                        </b>
                    </td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <xsl:template match="funct">
        <br/>
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <xsl:apply-templates/>
            <table align="center" width="98%">
                <tr>
                    <td>
                        <b>}</b>
                    </td>
                </tr>
                <tr>
                    <td bgcolor="#ffa500">[end of funct:&#160;<xsl:value-of
                            select="functHdr/name"/>()]</td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <xsl:template match="functHdr">
        <table align="center" width="98%" bgcolor="#ffa500">
            <tr>
                <td colspan="2" bgcolor="#cdc9a5">[C function]</td>
                <td colspan="2" bgcolor="#cdc9a5">
                    <xsl:value-of select="purp"/>
                </td>
            </tr>
            <tr>
                <td width="1%"/>
                <td width="13%">Name</td>
                <td width="85%">
                    <xsl:apply-templates select="fname"/>
                </td>
            </tr>
        </table>
        <xsl:if test="argList != &quot;&quot;">
            <xsl:apply-templates select="argList"/>
        </xsl:if>
        <xsl:apply-templates select="returnVal"/>
        <xsl:if test="descr != &quot;&quot;">
            <table align="center" width="98%" bgcolor="#ffa500">
                <tr>
                    <td width="1%"/>
                    <td width="13%">Description</td>
                    <td width="85%">
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </table>
        </xsl:if>
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td/>
                    <td/>
                    <td>
                        <b>
                            <xsl:apply-templates select="returnVal/arg/atype"
                                />&#160;<xsl:value-of select="cname"/>::<xsl:value-of
                                select="fname"/>
                            <xsl:for-each select="argList/arg">
                                <xsl:choose>
                                    <xsl:when test="position() =1">(</xsl:when>
                                </xsl:choose>
                                <xsl:apply-templates select="atype"/>&#160;<xsl:value-of
                                    select="aname"/>
                                <xsl:choose>
                                    <xsl:when test="position() = last()">)&#160;{</xsl:when>
                                    <xsl:otherwise>,</xsl:otherwise>
                                </xsl:choose>
                            </xsl:for-each>
                        </b>
                    </td>
                </tr>
            </table>
        </div>
    </xsl:template>
    <xsl:template match="code">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <br/>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="2" bgcolor="#cdc9a5">[code]</td>
                    <td colspan="2" bgcolor="#cdc9a5">
                        <xsl:value-of select="comment"/>
                    </td>
                </tr>
                <xsl:if test="tag != &quot;&quot;">
                    <tr>
                        <td width="1%"/>
                        <td width="13%">Tag</td>
                        <td width="85%">
                            <b>
                                <xsl:value-of select="tag"/>
                            </b>
                        </td>
                    </tr>
                </xsl:if>
            </table>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <!-- these templates have been processed in the "code" preamble already -->
    <xsl:template match="comment"/>
    <xsl:template match="tag"/>

    <xsl:template match="include">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
   <xsl:choose>
       <xsl:when test="fpath != &quot;&quot; or tag != &quot;&quot;">
                <br/>
                <table align="center" width="98%" bgcolor="#f5deb3">
                    <tr>
                        <td width="1%"/>
                        <td width="13%"/>
                        <td width="85%"/>
                    </tr>
                    <tr>
                        <td colspan="2" bgcolor="#cdc9a5">[include]</td>
                        <td colspan="2" bgcolor="#cdc9a5">
                            <xsl:if test="descr != &quot;&quot;">
                                <xsl:apply-templates select="descr"/>
                            </xsl:if>
                        </td>
                    </tr>
                    <xsl:if test="fpath != &quot;&quot;">
                        <tr>
                        <td width="1%"/>
                        <td width="13%">Path</td>
                        <td width="85%">
                            <b>
                                <xsl:value-of select="fpath"/>
                            </b>
                        </td>
                    </tr>
                        </xsl:if>
                    <xsl:if test="tag != &quot;&quot;">
                        <tr>
                            <td width="1%"/>
                            <td width="13%">Tag</td>
                            <td width="85%">
                                <b>
                                    <xsl:value-of select="tag"/>
                                </b>
                            </td>
                        </tr>
                    </xsl:if>
                </table>
                <table align="center" width="98%">
                    <tr>
                        <td bgcolor="#cdc9a5">[end &lt;include&gt;]</td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:otherwise>
                <table align="center" width="98%" bgcolor="#f5deb3">
                    <tr>
                        <td width="1%"/>
                        <td width="13%"/>
                        <td width="85%"/>
                    </tr>
                    <tr>
                    <td colspan="2" >[include]</td>
                    <td colspan="2" >
                        <xsl:if test="descr != &quot;&quot;">
                            <xsl:apply-templates select="descr"/>
                        </xsl:if>
                    </td>
                    </tr>
                    </table>
            </xsl:otherwise>
       </xsl:choose>
        </div>
    </xsl:template>

    <xsl:template match="foreach">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="2" bgcolor="#cdc9a5">[for each&#160;<b>
                            <xsl:value-of select="item"/>
                        </b>]</td>
                    <td bgcolor="#cdc9a5">
                        <xsl:if test="comment != &quot;&quot;">
                            <xsl:value-of select="comment"/>&#160;</xsl:if>
                        <xsl:if test="descr != &quot;&quot;">(<xsl:value-of select="descr"
                            />)</xsl:if>

                    </td>
                </tr>
                <xsl:if test="tag != &quot;&quot;">
                    <tr>
                        <td width="1%"/>
                        <td width="13%">Tag</td>
                        <td width="85%">
                            <b>
                                <xsl:value-of select="tag"/>
                            </b>
                        </td>
                    </tr>
                </xsl:if>
            </table>
            <xsl:apply-templates/>
            <table align="center" width="98%">
                <tr>
                    <td bgcolor="#cdc9a5">[end &lt;foreach&gt;]</td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <!-- these templates have been processed in the "foreach" preamble already -->
    <xsl:template match="foreach/item"/>
    <xsl:template match="foreach/descr"/>

    <xsl:template match="switch">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="3" bgcolor="#cdc9a5">[switch&#160;<b>
                            <xsl:value-of select="item"/>
                        </b>]</td>
                </tr>
            </table>
            <xsl:apply-templates select="case"/>
            <table align="center" width="98%">
                <tr>
                    <td bgcolor="#cdc9a5">[end &lt;switch&gt;]</td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <xsl:template match="case">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>+2%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="2" bgcolor="#cdc9a5">[case&#160;<b>
                            <xsl:value-of select="tag"/>
                        </b>]</td>
                    <td bgcolor="#cdc9a5">
                        <xsl:value-of select="descr"/>
                    </td>
                </tr>
            </table>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <!-- these templates have been processed in the "case" preamble already -->
    <xsl:template match="switch/case/tag"/>
    <xsl:template match="switch/case/descr"/>

    <xsl:template match="if">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <xsl:choose>
                        <xsl:when test="flagValue != &quot;&quot;">
                            <td colspan="2" bgcolor="#cdc9a5">[if&#160;<b><xsl:value-of
                                        select="flag"/>=<xsl:value-of select="flagValue"/></b>]</td>
                        </xsl:when>
                        <xsl:otherwise>
                            <td colspan="2" bgcolor="#cdc9a5">[if&#160;<b>
                                    <xsl:value-of select="flag"/>
                                </b>]</td>
                        </xsl:otherwise>
                    </xsl:choose>
                    <td bgcolor="#cdc9a5">
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </table>
            <xsl:apply-templates/>
            <table align="center" width="98%">
                <tr>
                    <td bgcolor="#cdc9a5">[end &lt;if&gt;]</td>
                </tr>
            </table>
        </div>
    </xsl:template>

    <!-- these templates have been processed in the "if" preamble already -->
    <xsl:template match="if/flag"/>
    <xsl:template match="if/flagValue"/>
    <xsl:template match="if/descr"/>

    <xsl:template match="elseif">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="2" bgcolor="#cdc9a5">[else if]</td>
                    <td bgcolor="#cdc9a5">
                        <b><xsl:value-of select="tag"/>: </b>
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </table>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="else">
        <div>
            <xsl:attribute name="style">margin-left: <xsl:value-of select="@indent"/>%</xsl:attribute>
            <table align="center" width="98%" bgcolor="#f5deb3">
                <tr>
                    <td width="1%"/>
                    <td width="13%"/>
                    <td width="85%"/>
                </tr>
                <tr>
                    <td colspan="3" bgcolor="#cdc9a5">[else]</td>
                </tr>
            </table>
            <xsl:apply-templates/>
        </div>
    </xsl:template>

    <xsl:template match="argList">
        <table align="center" width="98%" bgcolor="#ffa500">
            <xsl:for-each select="arg">
                <tr>
                    <td width="1%"/>
                    <td width="13%">
                        <xsl:choose>
                            <xsl:when test="position() =1">Arguments</xsl:when>
                        </xsl:choose>
                    </td>
                    <td width="20%"><xsl:apply-templates select="atype"/>&#160;<xsl:value-of
                            select="aname"/></td>
                    <td>
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </xsl:for-each>
        </table>
    </xsl:template>

    <xsl:template match="returnVal">
        <table align="center" width="98%" bgcolor="#ffa500">
            <tr>
                <td width="1%"/>
                <td width="13%">Return value</td>
                <td width="20%"><xsl:value-of select="arg/atype"/>&#160;<xsl:value-of
                        select="arg/aname"/></td>
                <td>
                    <xsl:value-of select="arg/descr"/>
                </td>
            </tr>
        </table>
    </xsl:template>

    <xsl:template match="rootClassList">
        <xsl:if test=". !=&quot;&quot;">
            <table align="center" width="98%" bgcolor="#f5deb3">
                <xsl:for-each select="classRef">
                    <xsl:sort select="cname" order="ascending" data-type="text"/>
                    <tr>
                        <td width="1%"/>
                        <td width="13%">
                            <xsl:choose>
                                <xsl:when test="position() =1">ROOT classes</xsl:when>
                            </xsl:choose>
                        </td>
                        <td width="13%">
                            <a>
                                <xsl:attribute name="href"
                                        >http://root.cern.ch/root/htmldoc/<xsl:value-of
                                        select="cname"/>.html</xsl:attribute>
                                <xsl:value-of select="cname"/>
                            </a>
                        </td>
                        <td>
                            <xsl:apply-templates select="descr"/>
                        </td>
                    </tr>
                </xsl:for-each>
            </table>
        </xsl:if>
    </xsl:template>

    <xsl:template match="mrbClassList">
        <xsl:if test=". !=&quot;&quot;">
            <table align="center" width="98%" bgcolor="#f5deb3">
                <xsl:for-each select="classRef">
                    <xsl:sort select="cname" order="ascending" data-type="text"/>
                    <tr>
                        <td width="1%"/>
                        <td width="13%">
                            <xsl:choose>
                                <xsl:when test="position() =1">MARaBOU classes</xsl:when>
                            </xsl:choose>
                        </td>
                        <td width="13%">
                            <a>
                                <xsl:attribute name="href"
                                        >http://www.bl.physik.uni-muenchen.de/marabou/html/<xsl:value-of
                                        select="cname"/>.html</xsl:attribute>
                                <xsl:value-of select="cname"/>
                            </a>
                        </td>
                        <td>
                            <xsl:apply-templates select="descr"/>
                        </td>
                    </tr>
                </xsl:for-each>
            </table>
        </xsl:if>
    </xsl:template>

    <xsl:template match="inheritance">
        <xsl:if test=". !=&quot;&quot;">
            <table align="center" width="98%" bgcolor="#f5deb3">
                <xsl:for-each select="classRef">
                    <xsl:sort select="cname" order="ascending" data-type="text"/>
                    <tr>
                        <td width="1%"/>
                        <td width="13%">
                            <xsl:choose>
                                <xsl:when test="position() =1">Parent classes</xsl:when>
                            </xsl:choose>
                        </td>
                        <td width="13%">
                            <xsl:if test="@type = &quot;root&quot;">
                                <a>
                                    <xsl:attribute name="href"
                                            >http://root.cern.ch/root/htmldoc/<xsl:value-of
                                            select="cname"/>.html</xsl:attribute>
                                    <xsl:value-of select="cname"/>
                                </a>
                            </xsl:if>
                            <xsl:if test="@type=&quot;mrb&quot;">
                                <a>
                                    <xsl:attribute name="href"
                                            >http://www.bl.physik.uni-muenchen.de/marabou/html/<xsl:value-of
                                            select="cname"/>.html</xsl:attribute>
                                    <xsl:value-of select="cname"/>
                                </a>
                            </xsl:if>
                        </td>
                        <td>
                            <xsl:apply-templates select="descr"/>
                        </td>
                    </tr>
                </xsl:for-each>
            </table>
        </xsl:if>
    </xsl:template>

    <xsl:template match="slist">
        <table align="center" width="98%" bgcolor="#f5deb3">
            <xsl:if test="@type=&quot;global&quot;">
                <tr>
                    <td colspan="2" bgcolor="#cdc9a5">[global section]</td>
                    <td colspan="2" bgcolor="#cdc9a5">
                        <xsl:apply-templates select="purp"/>
                    </td>
                </tr>
            </xsl:if>
            <xsl:for-each select="subst">
                <tr>
                    <td width="1%"/>
                    <td width="13%">
                        <xsl:if test="position() =1">Substitutions</xsl:if>
                    </td>
                    <td width="13%">
                        <xsl:value-of select="sname"/>
                    </td>
                    <td>
                        <xsl:apply-templates select="descr"/>
                    </td>
                </tr>
            </xsl:for-each>
        </table>
    </xsl:template>

    <xsl:template match="cbody">
        <xsl:if test=". != &quot;&quot;">
            <table align="center" width="98%">
                <xsl:apply-templates/>
            </table>
        </xsl:if>
        <xsl:if test=". = &quot;&quot;">
            <p/>
        </xsl:if>
    </xsl:template>

    <xsl:template match="l">
        <tr>
            <td width="16%"/>
            <td style="white-space: pre;">
                <xsl:apply-templates/>
            </td>
        </tr>
    </xsl:template>

    <xsl:template match="r">
        <a>
            <xsl:choose>
                <xsl:when test="@class != &quot;&quot;">
                    <xsl:attribute name="href">http://root.cern.ch/root/htmldoc/<xsl:value-of
                            select="@class"/>.html</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="href">http://root.cern.ch/root/htmldoc/<xsl:value-of
                            select="."/>.html</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:value-of select="."/>
        </a>
    </xsl:template>

    <xsl:template match="m">
        <a>
            <xsl:choose>
                <xsl:when test="@class != &quot;&quot;">
                    <xsl:attribute name="href"
                            >http://www.bl.physik.uni-muenchen.de/marabou/html/<xsl:value-of
                            select="@class"/>.html</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="href"
                            >http://www.bl.physik.uni-muenchen.de/marabou/html/<xsl:value-of
                            select="."/>.html</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:value-of select="."/>
        </a>
    </xsl:template>

    <xsl:template match="rm">
        <a>
            <xsl:attribute name="href">http://root.cern.ch/root/htmldoc/<xsl:value-of
                    select="@class"/>.html#<xsl:value-of select="@class"/>:<xsl:value-of select="."
                /></xsl:attribute>
            <xsl:value-of select="."/>
        </a>
    </xsl:template>

    <xsl:template match="mm">
        <a>
            <xsl:attribute name="href"
                    >http://www.bl.physik.uni-muenchen.de/marabou/html/<xsl:value-of select="@class"
                    />.html#<xsl:value-of select="@class"/>:<xsl:value-of select="."/></xsl:attribute>
            <xsl:value-of select="."/>
        </a>
    </xsl:template>

    <xsl:template match="s">
        <b title="to be substituted"
            style="color: blue; border-width: thin; border-style: solid; padding-left: 0.1em; padding-right: 0.1em;">
            <xsl:value-of select="."/>
        </b>
    </xsl:template>

    <xsl:template match="S">
        <b title="to be substituted (1st char upper case)"
            style="color: green; border-width: thin; border-style: solid; padding-left: 0.1em; padding-right: 0.1em;">
            <xsl:value-of select="."/>
        </b>
    </xsl:template>

    <xsl:template match="nl">
        <br/>
    </xsl:template>

</xsl:stylesheet>
