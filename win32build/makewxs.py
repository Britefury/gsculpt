##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import sys
import os

import xml.sax
import xml.sax.saxutils
import xml.parsers.expat

from Britefury.gSculptConfig import gSculptConfig




class Node (object):
    def __init__(self, name):
        self._children = []
        self._name = name
        self._attrs = {}
        self._content = ''

    def __getattr__(self, name):
        child = Node( name )
        self._children.append( child )
        return child

    def __call__(self, **attrs):
        self._attrs = attrs
        return self

    def __lshift__(self, content):
        self._content += content
        return self

    def _writeXml(self, handler):
        handler.startElement( self._name, self._attrs )
        if self._content != '':
            handler.characters( self._content )
        for child in self._children:
            child._writeXml( handler )
        handler.endElement( self._name )


gSculptNumericVersion = gSculptConfig.gSculptNumericVersionString

productName = 'gSculpt'
productId = '4c6c2e00-fe39-11da-92e3-0800200c9a66'
productVersion = gSculptNumericVersion
productVendor = 'Britefury'

productUpgradeCode = '52b18e90-fe39-11da-92e3-0800200c9a66'

minimumUpgradeVersion = '0.99.26'

versionString = gSculptConfig.gSculptVersionString

packageDescription = "Britefury's gSculpt installer"
packageComments = 'gSculpt by MrMeanie/Britefury'

mediaCabinet = 'gSculpt.cab'

gSculptRootComponentId = '5a2535f0-fe39-11da-92e3-0800200c9a66'
gSculptExamplesComponentId = '5fb85d80-fe39-11da-92e3-0800200c9a66'
gSculptGSImagesComponentId = '645a2030-fe39-11da-92e3-0800200c9a66'
gSculptStartMenuShortcutComponentId = '68ee0030-fe39-11da-92e3-0800200c9a66'
gSculptDesktopShortcutComponentId = '6e983230-fe39-11da-92e3-0800200c9a66'



componentIds = {
	'dist':                                                     '08ea7de0-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL':                                              '08ea7de1-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL':                                           '08ea7de2-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\ARB':                                       '08ea7de3-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\ATI':                                       '08ea7de4-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\Autodesk':                                  '08ea7de5-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\EXT':                                       '08ea7de6-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\HP':                                        '08ea7de7-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\IBM':                                       '08ea7de8-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\INTEL':                                     '08ea7de9-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\KTX':                                       '08ea7dea-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\MESA':                                      '08ea7deb-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\NV':                                        '08eaa4f0-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\PGI':                                       '08eaa4f1-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\SGI':                                       '08eaa4f2-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\SGIS':                                      '08eaa4f3-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\SGIX':                                      '08eaa4f4-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\SUN':                                       '08eaa4f5-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\SUNX':                                      '08eaa4f6-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\WIN':                                       '08eaa4f7-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\_3DFX':                                     '08eaa4f8-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\3DFX':                                      '08eacc00-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\APPLE':                                     '08eacc01-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\INGR':                                      '08eacc02-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\OML':                                       '08eacc03-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\REND':                                      '08eacc04-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GL\\S3':                                        '08eacc05-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GLU':                                          '08eacc06-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GLU\\EXT':                                      '08eacc07-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\GLU\\SGI':                                      '08eacc08-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\WGL':                                          '08eacc09-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\WGL\\ARB':                                      '08eacc0a-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\WGL\\EXT':                                      '08eacc0b-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\WGL\\_3DFX':                                    '08eacc0c-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\Tk':                                           '08eaf310-fe38-11da-92e3-0800200c9a66',
	'dist\\OpenGL\\Tk\\win32-tk8.4':                               '08eaf311-fe38-11da-92e3-0800200c9a66',
	'dist\\etc':                                                 '08eaf312-fe38-11da-92e3-0800200c9a66',
	'dist\\etc\\fonts':                                           '08ec0480-fe38-11da-92e3-0800200c9a66',
	'dist\\etc\\gtk-2.0':                                         '08ec0481-fe38-11da-92e3-0800200c9a66',
	'dist\\etc\\pango':                                           '08ec0482-fe38-11da-92e3-0800200c9a66',
	'dist\\lib':                                                 '08ec0483-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\gtk-2.0':                                         '08ec0484-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\gtk-2.0\\2.4.0':                                   '08ec0485-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\gtk-2.0\\2.4.0\\engines':                           '08ec2b90-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\gtk-2.0\\2.4.0\\immodules':                         '08ec2b91-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\gtk-2.0\\2.4.0\\loaders':                           '08ec2b92-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\pango':                                           '08ec2b93-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\pango\\1.5.0':                                     '1ab80849-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\pango\\1.5.0\\modules':                             '1ab8084a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\libglade':                                        '08ec2b96-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\libglade\\2.0':                                    '08ec2b97-fe38-11da-92e3-0800200c9a66',
	'dist\\lib\\locale':			'1ab8084e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\af':			'1ab8084f-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\af\\LC_MESSAGES':			'1ab82f50-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\am':			'1ab82f51-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\am\\LC_MESSAGES':			'1ab82f52-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ar':			'1ab82f53-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ar\\LC_MESSAGES':			'1ab82f54-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\as':			'1ab82f55-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\as\\LC_MESSAGES':			'1ab82f56-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\az':			'1ab82f57-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\az\\LC_MESSAGES':			'1ab82f58-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\be':			'1ab82f59-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\be\\LC_MESSAGES':			'1ab82f5a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bg':			'1ab82f5b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bg\\LC_MESSAGES':			'1ab82f5c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bn':			'1ab82f5d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bn\\LC_MESSAGES':			'1ab85660-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bs':			'1ab85661-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\bs\\LC_MESSAGES':			'1ab85662-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ca':			'1ab85663-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ca\\LC_MESSAGES':			'1ab85664-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\cs':			'1ab85665-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\cs\\LC_MESSAGES':			'1ab85666-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\cy':			'1ab85667-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\cy\\LC_MESSAGES':			'1ab85668-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\da':			'1ab85669-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\da\\LC_MESSAGES':			'1ab8566a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\de':			'1ab8566b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\de\\LC_MESSAGES':			'1ab8566c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\dz':			'1abb8ab5-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\dz\\LC_MESSAGES':			'1abb8ab6-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\el':			'1ab8566d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\el\\LC_MESSAGES':			'1ab87d70-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\en_CA':			'1ab87d71-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\en_CA\\LC_MESSAGES':			'1ab87d72-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\en_GB':			'1ab87d73-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\en_GB\\LC_MESSAGES':			'1ab87d74-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\eo':			'1ab87d75-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\eo\\LC_MESSAGES':			'1ab87d76-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\es':			'1ab87d77-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\es\\LC_MESSAGES':			'1ab87d78-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\et':			'1ab87d79-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\et\\LC_MESSAGES':			'1ab87d7a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\eu':			'1ab87d7b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\eu\\LC_MESSAGES':			'1ab87d7c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fa':			'1ab87d7d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fa\\LC_MESSAGES':			'1ab87d7e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fi':			'1ab87d7f-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fi\\LC_MESSAGES':			'1ab8a480-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fr':			'1ab8a481-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\fr\\LC_MESSAGES':			'1ab8a482-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ga':			'1ab8a483-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ga\\LC_MESSAGES':			'1ab8a484-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\gl':			'1ab8a485-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\gl\\LC_MESSAGES':			'1ab8a486-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\gu':			'1ab8a487-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\gu\\LC_MESSAGES':			'1ab8a488-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\he':			'1ab8a489-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\he\\LC_MESSAGES':			'1ab8a48a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hi':			'1ab8a48b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hi\\LC_MESSAGES':			'1ab8a48c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hr':			'1ab8a48d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hr\\LC_MESSAGES':			'1ab8a48e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hu':			'1ab8cb90-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hu\\LC_MESSAGES':			'1ab8cb91-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\id':			'1ab8cb92-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\id\\LC_MESSAGES':			'1ab8cb93-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\is':			'1ab8cb94-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\is\\LC_MESSAGES':			'1ab8cb95-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\it':			'1ab8cb96-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\it\\LC_MESSAGES':			'1ab8cb97-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ja':			'1ab8cb98-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ja\\LC_MESSAGES':			'1ab8cb99-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ka':			'1abb8ab7-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ka\\LC_MESSAGES':			'1abb8ab8-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\kn':			'1ab8cb9a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\kn\\LC_MESSAGES':			'1ab8cb9b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ko':			'1ab8cb9c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ko\\LC_MESSAGES':			'1ab8f2a0-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\li':			'1ab8f2a1-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\li\\LC_MESSAGES':			'1ab8f2a2-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\lt':			'1ab8f2a3-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\lt\\LC_MESSAGES':			'1ab8f2a4-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\lv':			'1ab8f2a5-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\lv\\LC_MESSAGES':			'1ab8f2a6-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mk':			'1ab8f2a7-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mk\\LC_MESSAGES':			'1aba7940-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ml':			'1aba7941-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ml\\LC_MESSAGES':			'1aba7942-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mn':			'1abaa050-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mn\\LC_MESSAGES':			'1abaa051-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mr':			'1abaa052-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mr\\LC_MESSAGES':			'1abaa053-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ms':			'1abaa054-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ms\\LC_MESSAGES':			'1abaa055-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nb':			'1abaa056-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nb\\LC_MESSAGES':			'1abaa057-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ne':			'1abaa058-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ne\\LC_MESSAGES':			'1abaa059-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nl':			'1abaa05a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nl\\LC_MESSAGES':			'1abaa05b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nn':			'1abaa05c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nn\\LC_MESSAGES':			'1abaa05d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\no':			'1abaa05e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\no\\LC_MESSAGES':			'1abac760-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pa':			'1abac761-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pa\\LC_MESSAGES':			'1abac762-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pl':			'1abac763-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pl\\LC_MESSAGES':			'1abac764-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pt':			'1abac765-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pt\\LC_MESSAGES':			'1abac766-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pt_BR':			'1abac767-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\pt_BR\\LC_MESSAGES':			'1abac768-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ro':			'1abac769-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ro\\LC_MESSAGES':			'1abac76a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ru':			'1abac76b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ru\\LC_MESSAGES':			'1abac76c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\rw':			'1abac76d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\rw\\LC_MESSAGES':			'1abac76e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sk':			'1abaee70-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sk\\LC_MESSAGES':			'1abaee71-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sl':			'1abaee72-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sl\\LC_MESSAGES':			'1abaee73-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sq':			'1abaee74-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sq\\LC_MESSAGES':			'1abaee75-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr':			'1abaee76-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr\\LC_MESSAGES':			'1abaee77-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr@Latn':			'1abaee78-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr@Latn\\LC_MESSAGES':			'1abaee79-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr@ije':			'1abaee7a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sr@ije\\LC_MESSAGES':			'1abaee7b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sv':			'1abaee7c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\sv\\LC_MESSAGES':			'1abaee7d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ta':			'1abaee7e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ta\\LC_MESSAGES':			'1abb1580-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\th':			'1abb1581-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\th\\LC_MESSAGES':			'1abb1582-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tk':			'1abb1583-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tk\\LC_MESSAGES':			'1abb1584-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tr':			'1abb1585-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tr\\LC_MESSAGES':			'1abb1586-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tt':			'1abb8ab9-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tt\\LC_MESSAGES':			'1abb8aba-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ug':			'1abb1587-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ug\\LC_MESSAGES':			'1abb1588-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uk':			'1abb1589-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uk\\LC_MESSAGES':			'1abb158a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\vi':			'1abb158b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\vi\\LC_MESSAGES':			'1abb158c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\wa':			'1abb158d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\wa\\LC_MESSAGES':			'1abb158e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\xh':			'1abb3c90-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\xh\\LC_MESSAGES':			'1abb3c91-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\yi':			'1abb3c92-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\yi\\LC_MESSAGES':			'1abb3c93-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_CN':			'1abb3c94-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_CN\\LC_MESSAGES':			'1abb3c95-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_TW':			'1abb3c96-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_TW\\LC_MESSAGES':			'1abb3c97-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\or':			'1abb3c98-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\or\\LC_MESSAGES':			'1abb3c99-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\te':			'1abb3c9a-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\te\\LC_MESSAGES':			'1abb3c9b-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tl':			'1abb3c9c-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\tl\\LC_MESSAGES':			'1abb3c9d-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_HK':			'1abb3c9e-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\zh_HK\\LC_MESSAGES':			'1abb63a0-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\az_IR':			'1abb63a1-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\az_IR\\LC_MESSAGES':			'1abb63a2-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\br':			'1abb63a3-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\br\\LC_MESSAGES':			'1abb63a4-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hy':			'1abb63a5-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\hy\\LC_MESSAGES':			'1abb63a6-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ia':			'1abb63a7-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ia\\LC_MESSAGES':			'1abb63a8-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ku':			'1abb63a9-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\ku\\LC_MESSAGES':			'1abb63aa-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mi':			'1abb63ab-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\mi\\LC_MESSAGES':			'1abb63ac-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nso':			'1abb63ad-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\nso\\LC_MESSAGES':			'1abb8ab0-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uz':			'1abb8ab1-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uz\\LC_MESSAGES':			'1abb8ab2-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uz@Latn':			'1abb8ab3-ddad-11da-8ad9-0800200c9a66',
	'dist\\lib\\locale\\uz@Latn\\LC_MESSAGES':			'1abb8ab4-ddad-11da-8ad9-0800200c9a66',
	'dist\\share':                                               '08ec2b98-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes':                                        '08ec2b99-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Default':                                '08ec2b9a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Default\\gtk-2.0-key':                    '08ec2b9b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Default\\gtk-2.0':                        '08ec2b9c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Emacs':                                  '08ec2b9d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Emacs\\gtk-2.0-key':                      '08ec2b9e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\MS-Windows':                             '08ec52a0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\MS-Windows\\gtk-2.0':                     '08ec52a1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Raleigh':                                '08ec52a2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Raleigh\\gtk-2.0':                        '08ec52a3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Metal':                                  '08ec52a4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Metal\\gtk-2.0':                          '08ec52a5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Redmond95':                              '08ec52a6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\themes\\Redmond95\\gtk-2.0':                      '08ec52a7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale':                                        '08ec52a8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\af':                                     '08ec52a9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\af\\LC_MESSAGES':                         '08ec52aa-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\am':                                     '08ec52ab-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\am\\LC_MESSAGES':                         '08ec52ac-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ar':                                     '08ec52ad-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ar\\LC_MESSAGES':                         '08ec79b0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\as':                                     '08ec79b1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\as\\LC_MESSAGES':                         '08ec79b2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\az':                                     '08ec79b3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\az\\LC_MESSAGES':                         '08ec79b4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\be':                                     '08ec79b5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\be\\LC_MESSAGES':                         '08ec79b6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bg':                                     '08ec79b7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bg\\LC_MESSAGES':                         '08ec79b8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bn':                                     '08ec79b9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bn\\LC_MESSAGES':                         '08ec79ba-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bs':                                     '08eca0c0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\bs\\LC_MESSAGES':                         '08eca0c1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ca':                                     '08eca0c2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ca\\LC_MESSAGES':                         '08eca0c3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\cs':                                     '08eca0c4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\cs\\LC_MESSAGES':                         '08eca0c5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\cy':                                     '08eca0c6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\cy\\LC_MESSAGES':                         '08eca0c7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\da':                                     '08eca0c8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\da\\LC_MESSAGES':                         '08eca0c9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\de':                                     '08eca0ca-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\de\\LC_MESSAGES':                         '08eca0cb-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\el':                                     '08eca0cc-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\el\\LC_MESSAGES':                         '08eca0cd-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\en_CA':                                  '08eca0ce-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\en_CA\\LC_MESSAGES':                      '08ecc7d0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\en_GB':                                  '08ecc7d1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\en_GB\\LC_MESSAGES':                      '08ecc7d2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\eo':                                     '08ecc7d3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\eo\\LC_MESSAGES':                         '08ecc7d4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\es':                                     '08ecc7d5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\es\\LC_MESSAGES':                         '08ecc7d6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\et':                                     '08ecc7d7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\et\\LC_MESSAGES':                         '08ecc7d8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\eu':                                     '08ecc7d9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\eu\\LC_MESSAGES':                         '08ecc7da-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fa':                                     '08ecc7db-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fa\\LC_MESSAGES':                         '08ecc7dc-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fi':                                     '08ecc7dd-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fi\\LC_MESSAGES':                         '08ecc7de-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fr':                                     '08eceee0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\fr\\LC_MESSAGES':                         '08eceee1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ga':                                     '08eceee2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ga\\LC_MESSAGES':                         '08eceee3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\gl':                                     '08eceee4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\gl\\LC_MESSAGES':                         '08eceee5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\gu':                                     '08eceee6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\gu\\LC_MESSAGES':                         '08eceee7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\he':                                     '08eceee8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\he\\LC_MESSAGES':                         '08eceee9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hi':                                     '08eceeea-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hi\\LC_MESSAGES':                         '08eceeeb-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hr':                                     '08eceeec-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hr\\LC_MESSAGES':                         '08ed15f0-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hu':                                     '08ed15f1-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hu\\LC_MESSAGES':                         '08ed15f2-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\id':                                     '08ed15f3-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\id\\LC_MESSAGES':                         '08ed15f4-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\is':                                     '08ed15f5-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\is\\LC_MESSAGES':                         '08ed15f6-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\it':                                     '08ed15f7-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\it\\LC_MESSAGES':                         '08ed15f8-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ja':                                     '08ed15f9-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ja\\LC_MESSAGES':                         '08ed3d00-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\kn':                                     '08ed3d01-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\kn\\LC_MESSAGES':                         '08ed3d02-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ko':                                     '08ed3d03-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ko\\LC_MESSAGES':                         '08ed3d04-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\li':                                     '08ed3d05-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\li\\LC_MESSAGES':                         '08ed3d06-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\lt':                                     '08ed3d07-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\lt\\LC_MESSAGES':                         '08ed3d08-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\lv':                                     '08ed3d09-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\lv\\LC_MESSAGES':                         '08ed3d0a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mk':                                     '08ed3d0b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mk\\LC_MESSAGES':                         '08ed3d0c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ml':                                     '08ed3d0d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ml\\LC_MESSAGES':                         '08ed3d0e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mn':                                     '08ed6410-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mn\\LC_MESSAGES':                         '08ed6411-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mr':                                     '08ed6412-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mr\\LC_MESSAGES':                         '08ed6413-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ms':                                     '08ed6414-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ms\\LC_MESSAGES':                         '08ed6415-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nb':                                     '08ed6416-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nb\\LC_MESSAGES':                         '08ed6417-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ne':                                     '08ed6418-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ne\\LC_MESSAGES':                         '08ed6419-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nl':                                     '08ed641a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nl\\LC_MESSAGES':                         '08ed641b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nn':                                     '08ed641c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nn\\LC_MESSAGES':                         '08ed641d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\no':                                     '08ed641e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\no\\LC_MESSAGES':                         '08ed8b20-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pa':                                     '08ed8b21-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pa\\LC_MESSAGES':                         '08ed8b22-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pl':                                     '08ed8b23-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pl\\LC_MESSAGES':                         '08ed8b24-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pt':                                     '08ed8b25-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pt\\LC_MESSAGES':                         '08ed8b26-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pt_BR':                                  '08ed8b27-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\pt_BR\\LC_MESSAGES':                      '08ed8b28-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ro':                                     '08ed8b29-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ro\\LC_MESSAGES':                         '08ed8b2a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ru':                                     '08ed8b2b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ru\\LC_MESSAGES':                         '08ed8b2c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\rw':                                     '08ed8b2d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\rw\\LC_MESSAGES':                         '08ed8b2e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sk':                                     '08ed8b2f-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sk\\LC_MESSAGES':                         '08edb230-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sl':                                     '08edb231-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sl\\LC_MESSAGES':                         '08edb232-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sq':                                     '08edb233-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sq\\LC_MESSAGES':                         '08edb234-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr':                                     '08edb235-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr\\LC_MESSAGES':                         '08edb236-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr@Latn':                                '08edb237-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr@Latn\\LC_MESSAGES':                    '08edb238-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr@ije':                                 '08edb239-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sr@ije\\LC_MESSAGES':                     '08edb23a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sv':                                     '08edb23b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\sv\\LC_MESSAGES':                         '08edb23c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ta':                                     '08edb23d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ta\\LC_MESSAGES':                         '08edd940-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\th':                                     '08edd941-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\th\\LC_MESSAGES':                         '08edd942-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tk':                                     '08edd943-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tk\\LC_MESSAGES':                         '08edd944-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tr':                                     '08edd945-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tr\\LC_MESSAGES':                         '08edd946-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ug':                                     '08edd947-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ug\\LC_MESSAGES':                         '08edd948-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uk':                                     '08edd949-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uk\\LC_MESSAGES':                         '08edd94a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\vi':                                     '08edd94b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\vi\\LC_MESSAGES':                         '08edd94c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\wa':                                     '08edd94d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\wa\\LC_MESSAGES':                         '08edd94e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\xh':                                     '08ee0050-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\xh\\LC_MESSAGES':                         '08ee0051-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\yi':                                     '08ee0052-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\yi\\LC_MESSAGES':                         '08ee0053-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_CN':                                  '08ee0054-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_CN\\LC_MESSAGES':                      '08ee0055-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_TW':                                  '08ee0056-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_TW\\LC_MESSAGES':                      '08ee0057-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\or':                                     '08ee0058-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\or\\LC_MESSAGES':                         '08ee0059-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\te':                                     '08ee005a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\te\\LC_MESSAGES':                         '08ee005b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tl':                                     '08ee005c-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\tl\\LC_MESSAGES':                         '08ee005d-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_HK':                                  '08ee005e-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\zh_HK\\LC_MESSAGES':                      '08ee2760-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\az_IR':                                  '08ee2761-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\az_IR\\LC_MESSAGES':                      '08ee2762-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\br':                                     '08ee2763-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\br\\LC_MESSAGES':                         '08ee2764-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hy':                                     '08ee2765-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\hy\\LC_MESSAGES':                         '08ee2766-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ia':                                     '08ee2767-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ia\\LC_MESSAGES':                         '08ee2768-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ku':                                     '08ee2769-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\ku\\LC_MESSAGES':                         '08ee276a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mi':                                     '08ee276b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\mi\\LC_MESSAGES':                         '08ee4e70-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nso':                                    '08ee4e71-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\nso\\LC_MESSAGES':                        '08ee4e72-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uz':                                     '08ee4e73-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uz\\LC_MESSAGES':                         '08ee4e74-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uz@Latn':                                '08ee4e75-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\locale\\uz@Latn\\LC_MESSAGES':                    '08ee4e76-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\gtk-2.0':                                       '08ee4e77-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\gtk-2.0\\demo':                                  '08ee4e78-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\xml':                                           '08ee4e79-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\xml\\libglade':                                  '08ee4e7a-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\gtkthemeselector':                              '08ee4e7b-fe38-11da-92e3-0800200c9a66',
	'dist\\share\\gtkthemeselector\\pixmaps':                      '08ee7580-fe38-11da-92e3-0800200c9a66',
	'dist\\gsimages':                                            '08ee7581-fe38-11da-92e3-0800200c9a66',
	'dist\\examples':                                            '08ee7582-fe38-11da-92e3-0800200c9a66',
	'dist\\locale':							'1ab8084b-ddad-11da-8ad9-0800200c9a66',
	'dist\\locale\\tr':							'1ab8084c-ddad-11da-8ad9-0800200c9a66',
	'dist\\locale\\tr\\LC_MESSAGES':			'1ab8084d-ddad-11da-8ad9-0800200c9a66',
	'dist\\locale\\hu':							'1abb8abc-ddad-11da-8ad9-0800200c9a66',
	'dist\\locale\\hu\\LC_MESSAGES':			'1abbb1c0-ddad-11da-8ad9-0800200c9a66',
	'dist\\doc':								'1abb8abb-ddad-11da-8ad9-0800200c9a66'
}

componentIds = dict( [ ( dir.lower(), id )   for dir, id in componentIds.items() ] )


def fixId(id):
    return id.replace( '.', '_' ).replace( '-', '_' ).replace( '@', '_' )


if len( sys.argv ) != 2:
    print 'Usage:'
    print '\t%s <out_filename>' % ( sys.argv[0], )
else:
    def gatherExamples(gSculptExamplesComponent):
        for dirpath, dirnames, filenames in os.walk( os.path.join( 'dist', 'examples' ) ):
            filenames = [ filename   for filename in filenames   if filename.endswith( '.gpx' ) ]
            for filename in filenames:
                base, ext = os.path.splitext( filename )
                gSculptExamplesComponent.File( Id='Example' + base + '_' + ext[1:], Name=base[:8]+ext[:4], LongName=filename, Source=os.path.join( os.getcwd(), 'dist', 'examples', filename ), Vital='yes', DiskId='1' )

    def gatherRootContents(gSculptRootComponent):
        for filename in os.listdir( 'dist' ):
            if os.path.isfile( os.path.join( 'dist', filename ) )  and  ( filename.endswith( '.dll' )  or  filename.endswith( '.pyd' )  or  filename.endswith( '.zip' ) ):
                base, ext = os.path.splitext( filename )
                gSculptRootComponent.File( Id=fixId( base ) + '_' + ext[1:], Name=base[:8]+ext[:4], LongName=filename, Source=os.path.join( os.getcwd(), 'dist', filename ), Vital='yes', DiskId='1' )

    def gatherRecursive(parentDirNode, dirName, id, dirPath):
        if len( dirName ) > 8:
            dirNode = parentDirNode.Directory( Id=fixId( id ), Name=dirName[:8], LongName=dirName )
        else:
            dirNode = parentDirNode.Directory( Id=fixId( id ), Name=dirName )
        componentId = id + 'Component'
        componentId = fixId( componentId )
        componentNode = dirNode.Component( Id=componentId , Guid=componentIds[dirPath.lower()] )
        componentIdList = [ componentId ]
        for entry in os.listdir( dirPath ):
            entryPath = os.path.join( dirPath, entry )
            if os.path.isdir( entryPath ):
                componentIdList.extend( gatherRecursive( dirNode, entry, id + '_' + entry, entryPath ) )
            elif os.path.isfile( entryPath ):
                base, ext = os.path.splitext( entry )
                fileId = id + '_' + base + '_' + ext[1:]
                componentNode.File( Id=fixId( fileId ), Name=base[:8]+ext[:4], LongName=entry, Source=os.path.join( os.getcwd(), entryPath ), Vital='yes', DiskId='1' )
        return componentIdList

    def gatherGSImages(gSculptGSImagesComponent):
        for filename in os.listdir( os.path.join( 'dist', 'gsimages' ) ):
            filePath = os.path.join( 'dist', 'gsimages', filename )
            if os.path.isfile( filePath ):
                base, ext = os.path.splitext( filename )
                gSculptGSImagesComponent.File( Id='gsimage_' + base.replace( '-', '_' ) + '_' + ext[1:], Name=base[:8]+ext[:4], LongName=filename, Source=os.path.join( os.getcwd(), 'dist', 'gsimages', filename ), Vital='yes', DiskId='1' )



    wix = Node( 'Wix' )( xmlns='http://schemas.microsoft.com/wix/2003/01/wi' )
    product = wix.Product( Name=productName, Id=productId, UpgradeCode=productUpgradeCode, Language='1033', Codepage='1252', Version=productVersion, Manufacturer=productVendor )
    package = product.Package( Id='????????-????-????-????-????????????', Keywords='Installer', Description=packageDescription, Comments=packageComments, InstallerVersion='100',
                               Languages='1033', Compressed='yes', SummaryCodepage='1252' )
    upgrade = product.Upgrade( Id=productUpgradeCode )
    upgrade.UpgradeVersion( OnlyDetect='yes', Property='PATCHFOUND', IncludeMinimum='yes', Minimum=productVersion, IncludeMaximum='yes', Maximum=productVersion )
    upgrade.UpgradeVersion( OnlyDetect='yes', Property='NEWERFOUND', IncludeMinimum='no', Minimum=productVersion )
    media = product.Media( Id='1', Cabinet=mediaCabinet, EmbedCab='yes' )
    product.Condition( Message='gSculpt required Windows 2000 or above' )  <<  'VersionNT >= 500'
    targetDir = product.Directory( Id='TARGETDIR', Name='SourceDir' )
    progFilesDir = targetDir.Directory( Id='ProgramFilesFolder', Name='PFiles' )
    britefuryDir = progFilesDir.Directory( Id='Britefury', Name='Brite', LongName='Britefury' )
    gSculptDir = britefuryDir.Directory( Id='INSTALLDIR', Name='gSculpt' )
    examplesDir = gSculptDir.Directory( Id='examples', Name='examples' )
    gsImagesDir = gSculptDir.Directory( Id='gsimages', Name='gsimages' )
    gSculptRootComponent = gSculptDir.Component( Id='gSculptRootComponent', Guid=gSculptRootComponentId )

    etcComponentIds = gatherRecursive( gSculptDir, 'etc', 'etc', os.path.join( 'dist', 'etc' ) )
    libComponentIds = gatherRecursive( gSculptDir, 'lib', 'lib', os.path.join( 'dist', 'lib' ) )
    openGLComponentIds = gatherRecursive( gSculptDir, 'OpenGL', 'opengl', os.path.join( 'dist', 'OpenGL' ) )
    shareComponentIds = gatherRecursive( gSculptDir, 'share', 'share', os.path.join( 'dist', 'share' ) )
    localeComponentIds = gatherRecursive( gSculptDir, 'locale', 'locale', os.path.join( 'dist', 'locale' ) )
    docComponentIds = gatherRecursive( gSculptDir, 'doc', 'doc', os.path.join( 'dist', 'doc' ) )
    allComponentIds = etcComponentIds + libComponentIds + openGLComponentIds + shareComponentIds + localeComponentIds + docComponentIds

    gatherRootContents( gSculptRootComponent )

    gSculptRootComponent.File( Id='AUTHORS', Name='AUTHORS', Source=os.path.join( os.getcwd(), 'AUTHORS' ), DiskId='1' )
    gSculptRootComponent.File( Id='AUTHORS_TRANSLATIONS', Name='AUTHXLAT', LongName='AUTHORS_TRANSLATIONS', Source=os.path.join( os.getcwd(), 'AUTHORS_TRANSLATIONS' ), DiskId='1' )
    gSculptRootComponent.File( Id='ChangeLog', Name='ChangeLg', LongName='ChangeLog', Source=os.path.join( os.getcwd(), 'ChangeLog' ), DiskId='1' )
    gSculptRootComponent.File( Id='COPYING', Name='COPYING', Source=os.path.join( os.getcwd(), 'COPYING' ), DiskId='1' )
    gSculptRootComponent.File( Id='LICENSE', Name='LICENSE', Source=os.path.join( os.getcwd(), 'LICENSE' ), DiskId='1' )
    gSculptRootComponent.File( Id='w9xpopenexe', Name='w9xpopen.exe', Source=os.path.join( os.getcwd(), 'dist', 'w9xpopen.exe' ), DiskId='1' )
    mainExeFile = gSculptRootComponent.File( Id='gsculpt_exe', Name='gSculpt.exe', Source=os.path.join( os.getcwd(), 'dist', 'gSculpt.exe' ), Vital='yes', DiskId='1' )
    mainExeFile.Shortcut( Id='startMenuGSculpt', Directory='ProgramMenuDir', Name='gSculpt', LongName='gSculpt ' + versionString, WorkingDirectory='INSTALLDIR' )
    mainExeFile.Shortcut( Id='desktopGSculpt', Directory='DesktopFolder', Name='gSculpt', LongName='gSculpt ' + versionString, WorkingDirectory='INSTALLDIR' )
    gSculptProgId = gSculptRootComponent.ProgId( Id='gSculpt.gpxfile', Description='gSculpt project file' )
    gSculptGPXExt = gSculptProgId.Extension( Id='gpx', ContentType='application/gpx' )
    gSculptGPXExt.Verb( Id='open', Sequence='10', Command='Open', Target='[!gsculpt_exe]', Argument='"%1"' )
    gSculptRootComponent.Registry( Id='gSculptIcon1', Root='HKCR', Key='.gpx', Action='write', Type='string', Value='gSculpt.gpxfile' )
    gSculptRootComponent.Registry( Id='gSculptIcon2', Root='HKCR', Key='gSculpt.gpxfile', Action='write', Type='string', Value='gSculpt project file' )
    gSculptRootComponent.Registry( Id='gSculptIcon3', Root='HKCR', Key='gSculpt.gpxfile\\DefaultIcon', Action='write', Type='string', Value='[INSTALLDIR]gSculpt.exe,0' )

    gSculptExamplesComponent = examplesDir.Component( Id='gSculptExamplesComponent', Guid=gSculptExamplesComponentId )
    gatherExamples( gSculptExamplesComponent )

    gSculptGSImagesComponent = gsImagesDir.Component( Id='gSculptGSImagesComponent', Guid=gSculptGSImagesComponentId )
    gatherGSImages( gSculptGSImagesComponent )


    programMenuDir = targetDir.Directory( Id='ProgramMenuFolder', Name='PMenu', LongName='Programs' )
    gSculptStartMenuDIr = programMenuDir.Directory( Id='ProgramMenuDir', Name='gSculpt', LongName='gSculpt ' + versionString )

    targetDir.Directory( Id='DesktopFolder', Name='Desktop' )



    completeFeature = product.Feature( Id='Complete', Title='gSculpt', Description='The complete package', Display='expand', Level='1', ConfigurableDirectory='INSTALLDIR' )
    binFeature = completeFeature.Feature( Id='binFeature', Title='Main program', Description='The main gSculpt program', Level='1' )
    binFeature.ComponentRef( Id='gSculptRootComponent' )
    binFeature.ComponentRef( Id='gSculptGSImagesComponent' )
    for componentId in allComponentIds:
        binFeature.ComponentRef( Id=componentId )
    examplesFeature = completeFeature.Feature( Id='examplesFeature', Title='Examples', Description='gSculpt examples', Level='1' )
    examplesFeature.ComponentRef( Id='gSculptExamplesComponent' )


    product.UIRef( Id='WixUI_Mondo' )
    product.UIRef( Id='WixUI_ErrorProgressText' )

    product.CustomAction( Id='AlreadyUpdated', Error='%s has already been updated to %s or newer' % ( productName, gSculptNumericVersion ) )
    product.CustomAction( Id='NoDowngrade', Error='A later version of %s has already been installed' % ( productName, ) )

    installExecuteSequence = product.InstallExecuteSequence()
    installExecuteSequence.Custom( Action='AlreadyUpdated', After='FindRelatedProducts' )  <<  'PATCHFOUND'
    installExecuteSequence.Custom( Action='NoDowngrade', After='FindRelatedProducts' )  <<  'NEWERFOUND'
    installExecuteSequence.RemoveExistingProducts( After='InstallFinalize' )

    f = open( sys.argv[1], 'w' )
    generator = xml.sax.saxutils.XMLGenerator( f )
    generator.startDocument()
    wix._writeXml( generator )
    generator.endDocument()
    f.close()






# A bunch of GUIDs.
# Take one and remove it from the list each time one is needed.
##1abbb1c1-ddad-11da-8ad9-0800200c9a66
##1abbb1c2-ddad-11da-8ad9-0800200c9a66
##1abbb1c3-ddad-11da-8ad9-0800200c9a66
##1abbb1c4-ddad-11da-8ad9-0800200c9a66
##1abbb1c5-ddad-11da-8ad9-0800200c9a66
##1abbb1c6-ddad-11da-8ad9-0800200c9a66
##1abbb1c7-ddad-11da-8ad9-0800200c9a66
##1abbb1c8-ddad-11da-8ad9-0800200c9a66
##1abbb1c9-ddad-11da-8ad9-0800200c9a66
##1abbb1ca-ddad-11da-8ad9-0800200c9a66
##1abbd8d0-ddad-11da-8ad9-0800200c9a66
##1abbd8d1-ddad-11da-8ad9-0800200c9a66
##1abbd8d2-ddad-11da-8ad9-0800200c9a66
##1abbd8d3-ddad-11da-8ad9-0800200c9a66
##1abbd8d4-ddad-11da-8ad9-0800200c9a66
##1abbd8d5-ddad-11da-8ad9-0800200c9a66
##1abbd8d6-ddad-11da-8ad9-0800200c9a66
##1abbd8d7-ddad-11da-8ad9-0800200c9a66
##1abbd8d8-ddad-11da-8ad9-0800200c9a66
##1abbd8d9-ddad-11da-8ad9-0800200c9a66
##1abbd8da-ddad-11da-8ad9-0800200c9a66
##1abbd8db-ddad-11da-8ad9-0800200c9a66
##1abbd8dc-ddad-11da-8ad9-0800200c9a66
##1abbd8dd-ddad-11da-8ad9-0800200c9a66
##1abbd8de-ddad-11da-8ad9-0800200c9a66
##1abbd8df-ddad-11da-8ad9-0800200c9a66
##1abbffe0-ddad-11da-8ad9-0800200c9a66
##1abbffe1-ddad-11da-8ad9-0800200c9a66
##1abbffe2-ddad-11da-8ad9-0800200c9a66
##1abbffe3-ddad-11da-8ad9-0800200c9a66
##1abbffe4-ddad-11da-8ad9-0800200c9a66
##1abbffe5-ddad-11da-8ad9-0800200c9a66
##1abbffe6-ddad-11da-8ad9-0800200c9a66
##1abbffe7-ddad-11da-8ad9-0800200c9a66
##1abbffe8-ddad-11da-8ad9-0800200c9a66
##1abbffe9-ddad-11da-8ad9-0800200c9a66
##1abbffea-ddad-11da-8ad9-0800200c9a66
##1abbffeb-ddad-11da-8ad9-0800200c9a66
##1abbffec-ddad-11da-8ad9-0800200c9a66
##1abbffed-ddad-11da-8ad9-0800200c9a66
##1abc26f0-ddad-11da-8ad9-0800200c9a66
##1abc26f1-ddad-11da-8ad9-0800200c9a66
##1abc26f2-ddad-11da-8ad9-0800200c9a66
##1abc26f3-ddad-11da-8ad9-0800200c9a66
##1abc26f4-ddad-11da-8ad9-0800200c9a66
##1abc26f5-ddad-11da-8ad9-0800200c9a66
##1abc26f6-ddad-11da-8ad9-0800200c9a66
##1abc26f7-ddad-11da-8ad9-0800200c9a66
##1abc26f8-ddad-11da-8ad9-0800200c9a66
##1abc26f9-ddad-11da-8ad9-0800200c9a66
##1abc26fa-ddad-11da-8ad9-0800200c9a66
##1abc26fb-ddad-11da-8ad9-0800200c9a66
##1abc26fc-ddad-11da-8ad9-0800200c9a66
##1abc26fd-ddad-11da-8ad9-0800200c9a66
##1abc26fe-ddad-11da-8ad9-0800200c9a66
##1abc4e00-ddad-11da-8ad9-0800200c9a66
##1abc4e01-ddad-11da-8ad9-0800200c9a66
##1abc4e02-ddad-11da-8ad9-0800200c9a66
##1abc4e03-ddad-11da-8ad9-0800200c9a66
##1abc4e04-ddad-11da-8ad9-0800200c9a66
##1abc4e05-ddad-11da-8ad9-0800200c9a66
##1abc4e06-ddad-11da-8ad9-0800200c9a66
##1abc4e07-ddad-11da-8ad9-0800200c9a66
##1abc4e08-ddad-11da-8ad9-0800200c9a66
##1abc4e09-ddad-11da-8ad9-0800200c9a66
##1abc4e0a-ddad-11da-8ad9-0800200c9a66
##1abc4e0b-ddad-11da-8ad9-0800200c9a66
##1abc4e0c-ddad-11da-8ad9-0800200c9a66
##1abc4e0d-ddad-11da-8ad9-0800200c9a66
##1abc4e0e-ddad-11da-8ad9-0800200c9a66
##1abc4e0f-ddad-11da-8ad9-0800200c9a66
##1abc7510-ddad-11da-8ad9-0800200c9a66
##1abc7511-ddad-11da-8ad9-0800200c9a66
##1abc7512-ddad-11da-8ad9-0800200c9a66
##1abc7513-ddad-11da-8ad9-0800200c9a66
##1abc7514-ddad-11da-8ad9-0800200c9a66
##1abc7515-ddad-11da-8ad9-0800200c9a66
##1abc7516-ddad-11da-8ad9-0800200c9a66
##1abc7517-ddad-11da-8ad9-0800200c9a66
##1abc7518-ddad-11da-8ad9-0800200c9a66
##1abc7519-ddad-11da-8ad9-0800200c9a66
##1abc751a-ddad-11da-8ad9-0800200c9a66
##1abc751b-ddad-11da-8ad9-0800200c9a66
##1abc751c-ddad-11da-8ad9-0800200c9a66
##1abc751d-ddad-11da-8ad9-0800200c9a66
##1abc751e-ddad-11da-8ad9-0800200c9a66
##1abc9c20-ddad-11da-8ad9-0800200c9a66
##1abc9c21-ddad-11da-8ad9-0800200c9a66
##1abc9c22-ddad-11da-8ad9-0800200c9a66
##1abc9c23-ddad-11da-8ad9-0800200c9a66
##1abc9c24-ddad-11da-8ad9-0800200c9a66
##1abc9c25-ddad-11da-8ad9-0800200c9a66
##1abc9c26-ddad-11da-8ad9-0800200c9a66
##1abc9c27-ddad-11da-8ad9-0800200c9a66
##1abc9c28-ddad-11da-8ad9-0800200c9a66
##1abc9c29-ddad-11da-8ad9-0800200c9a66
##1abc9c2a-ddad-11da-8ad9-0800200c9a66
##1abc9c2b-ddad-11da-8ad9-0800200c9a66
##1abc9c2c-ddad-11da-8ad9-0800200c9a66
##1abcc330-ddad-11da-8ad9-0800200c9a66
##1abcc331-ddad-11da-8ad9-0800200c9a66
##1abcc332-ddad-11da-8ad9-0800200c9a66
##1abcc333-ddad-11da-8ad9-0800200c9a66
##1abcc334-ddad-11da-8ad9-0800200c9a66
##1abcc335-ddad-11da-8ad9-0800200c9a66
##1abcc336-ddad-11da-8ad9-0800200c9a66
##1abcc337-ddad-11da-8ad9-0800200c9a66
##1abcc338-ddad-11da-8ad9-0800200c9a66
##1abcc339-ddad-11da-8ad9-0800200c9a66
##1abcc33a-ddad-11da-8ad9-0800200c9a66
##1abcc33b-ddad-11da-8ad9-0800200c9a66
##1abcea40-ddad-11da-8ad9-0800200c9a66
##1abcea41-ddad-11da-8ad9-0800200c9a66
##1abcea42-ddad-11da-8ad9-0800200c9a66
##1abcea43-ddad-11da-8ad9-0800200c9a66
##1abcea44-ddad-11da-8ad9-0800200c9a66
##1abcea45-ddad-11da-8ad9-0800200c9a66
##1abcea46-ddad-11da-8ad9-0800200c9a66
##1abcea47-ddad-11da-8ad9-0800200c9a66
##1abcea48-ddad-11da-8ad9-0800200c9a66
##1abcea49-ddad-11da-8ad9-0800200c9a66
##1abcea4a-ddad-11da-8ad9-0800200c9a66
##1abcea4b-ddad-11da-8ad9-0800200c9a66
##1abcea4c-ddad-11da-8ad9-0800200c9a66
##1abcea4d-ddad-11da-8ad9-0800200c9a66
##1abcea4e-ddad-11da-8ad9-0800200c9a66
##1abcea4f-ddad-11da-8ad9-0800200c9a66
##1abd1150-ddad-11da-8ad9-0800200c9a66
##1abd1151-ddad-11da-8ad9-0800200c9a66
##1abd1152-ddad-11da-8ad9-0800200c9a66
##1abd1153-ddad-11da-8ad9-0800200c9a66
##1abd1154-ddad-11da-8ad9-0800200c9a66
##1abd1155-ddad-11da-8ad9-0800200c9a66
##1abd1156-ddad-11da-8ad9-0800200c9a66
##1abd1157-ddad-11da-8ad9-0800200c9a66
##1abd1158-ddad-11da-8ad9-0800200c9a66
##1abd1159-ddad-11da-8ad9-0800200c9a66
##1abd115a-ddad-11da-8ad9-0800200c9a66
##1abd115b-ddad-11da-8ad9-0800200c9a66
##1abd115c-ddad-11da-8ad9-0800200c9a66
##1abd115d-ddad-11da-8ad9-0800200c9a66
##1abd115e-ddad-11da-8ad9-0800200c9a66
##1abd3860-ddad-11da-8ad9-0800200c9a66
##1abd3861-ddad-11da-8ad9-0800200c9a66
##1abd3862-ddad-11da-8ad9-0800200c9a66
##1abd3863-ddad-11da-8ad9-0800200c9a66
##1abd3864-ddad-11da-8ad9-0800200c9a66
##1abd3865-ddad-11da-8ad9-0800200c9a66
##1abd3866-ddad-11da-8ad9-0800200c9a66
##1abd3867-ddad-11da-8ad9-0800200c9a66
##1abd3868-ddad-11da-8ad9-0800200c9a66
##1abd3869-ddad-11da-8ad9-0800200c9a66
##1abd386a-ddad-11da-8ad9-0800200c9a66
##1abd386b-ddad-11da-8ad9-0800200c9a66
##1abd386c-ddad-11da-8ad9-0800200c9a66
##1abd5f70-ddad-11da-8ad9-0800200c9a66
##1abd5f71-ddad-11da-8ad9-0800200c9a66
##1abd5f72-ddad-11da-8ad9-0800200c9a66
##1abd5f73-ddad-11da-8ad9-0800200c9a66
##1abd5f74-ddad-11da-8ad9-0800200c9a66
##1abd5f75-ddad-11da-8ad9-0800200c9a66
##1abd5f76-ddad-11da-8ad9-0800200c9a66
##1abd5f77-ddad-11da-8ad9-0800200c9a66
##1abd5f78-ddad-11da-8ad9-0800200c9a66
##1abd5f79-ddad-11da-8ad9-0800200c9a66
##1abd5f7a-ddad-11da-8ad9-0800200c9a66
##1abd5f7b-ddad-11da-8ad9-0800200c9a66
##1abd5f7c-ddad-11da-8ad9-0800200c9a66
##1abd8680-ddad-11da-8ad9-0800200c9a66
##1abd8681-ddad-11da-8ad9-0800200c9a66
##1abd8682-ddad-11da-8ad9-0800200c9a66
##1abd8683-ddad-11da-8ad9-0800200c9a66
##1abd8684-ddad-11da-8ad9-0800200c9a66
##1abd8685-ddad-11da-8ad9-0800200c9a66
##1abd8686-ddad-11da-8ad9-0800200c9a66
##1abd8687-ddad-11da-8ad9-0800200c9a66
##1abd8688-ddad-11da-8ad9-0800200c9a66
##1abd8689-ddad-11da-8ad9-0800200c9a66
##1abd868a-ddad-11da-8ad9-0800200c9a66
##1abd868b-ddad-11da-8ad9-0800200c9a66
##1abd868c-ddad-11da-8ad9-0800200c9a66
##1abd868d-ddad-11da-8ad9-0800200c9a66
##1abd868e-ddad-11da-8ad9-0800200c9a66
##1abdad90-ddad-11da-8ad9-0800200c9a66
##1abdad91-ddad-11da-8ad9-0800200c9a66
##1abdad92-ddad-11da-8ad9-0800200c9a66
##1abdad93-ddad-11da-8ad9-0800200c9a66
##1abdad94-ddad-11da-8ad9-0800200c9a66
##1abdad95-ddad-11da-8ad9-0800200c9a66
##1abdad96-ddad-11da-8ad9-0800200c9a66
##1abdad97-ddad-11da-8ad9-0800200c9a66
##1abdad98-ddad-11da-8ad9-0800200c9a66
##1abdad99-ddad-11da-8ad9-0800200c9a66
##1abdad9a-ddad-11da-8ad9-0800200c9a66
##1abdad9b-ddad-11da-8ad9-0800200c9a66
##1abdad9c-ddad-11da-8ad9-0800200c9a66
##1abdad9d-ddad-11da-8ad9-0800200c9a66
##1abdad9e-ddad-11da-8ad9-0800200c9a66
##1abdd4a0-ddad-11da-8ad9-0800200c9a66
##1abdd4a1-ddad-11da-8ad9-0800200c9a66
##1abdd4a2-ddad-11da-8ad9-0800200c9a66
##1abdd4a3-ddad-11da-8ad9-0800200c9a66
##1abdd4a4-ddad-11da-8ad9-0800200c9a66
##1abdd4a5-ddad-11da-8ad9-0800200c9a66
##1abdd4a6-ddad-11da-8ad9-0800200c9a66
##1abdd4a7-ddad-11da-8ad9-0800200c9a66
##1abdd4a8-ddad-11da-8ad9-0800200c9a66
##1abdd4a9-ddad-11da-8ad9-0800200c9a66
##1abdd4aa-ddad-11da-8ad9-0800200c9a66
##1abdd4ab-ddad-11da-8ad9-0800200c9a66
##1abdd4ac-ddad-11da-8ad9-0800200c9a66
##1abdd4ad-ddad-11da-8ad9-0800200c9a66
##1abdd4ae-ddad-11da-8ad9-0800200c9a66
##1abdfbb0-ddad-11da-8ad9-0800200c9a66
##1abdfbb1-ddad-11da-8ad9-0800200c9a66
##1abdfbb2-ddad-11da-8ad9-0800200c9a66
##1abdfbb3-ddad-11da-8ad9-0800200c9a66
##1abdfbb4-ddad-11da-8ad9-0800200c9a66
##1abdfbb5-ddad-11da-8ad9-0800200c9a66
##1abdfbb6-ddad-11da-8ad9-0800200c9a66
##1abdfbb7-ddad-11da-8ad9-0800200c9a66
##1abdfbb8-ddad-11da-8ad9-0800200c9a66
##1abdfbb9-ddad-11da-8ad9-0800200c9a66
##1abdfbba-ddad-11da-8ad9-0800200c9a66
##1abdfbbb-ddad-11da-8ad9-0800200c9a66
##1abdfbbc-ddad-11da-8ad9-0800200c9a66
##1abdfbbd-ddad-11da-8ad9-0800200c9a66
##1abdfbbe-ddad-11da-8ad9-0800200c9a66
##1abe22c0-ddad-11da-8ad9-0800200c9a66
##1abe22c1-ddad-11da-8ad9-0800200c9a66
##1abe22c2-ddad-11da-8ad9-0800200c9a66
##1abe22c3-ddad-11da-8ad9-0800200c9a66
##1abe22c4-ddad-11da-8ad9-0800200c9a66
##1abe22c5-ddad-11da-8ad9-0800200c9a66
##1abe22c6-ddad-11da-8ad9-0800200c9a66
##1abe22c7-ddad-11da-8ad9-0800200c9a66
##1abe49d0-ddad-11da-8ad9-0800200c9a66
##1abe49d1-ddad-11da-8ad9-0800200c9a66
##1abe49d2-ddad-11da-8ad9-0800200c9a66
##1abe49d3-ddad-11da-8ad9-0800200c9a66
##1abe49d4-ddad-11da-8ad9-0800200c9a66
##1abe49d5-ddad-11da-8ad9-0800200c9a66
##1abe49d6-ddad-11da-8ad9-0800200c9a66
##1abe49d7-ddad-11da-8ad9-0800200c9a66
##1abe49d8-ddad-11da-8ad9-0800200c9a66
##1abe70e0-ddad-11da-8ad9-0800200c9a66
##1abe70e1-ddad-11da-8ad9-0800200c9a66
##1abe70e2-ddad-11da-8ad9-0800200c9a66
##1abe70e3-ddad-11da-8ad9-0800200c9a66
##1abe70e4-ddad-11da-8ad9-0800200c9a66
##1abe70e5-ddad-11da-8ad9-0800200c9a66
##1abe70e6-ddad-11da-8ad9-0800200c9a66
##1abe70e7-ddad-11da-8ad9-0800200c9a66
##1abe70e8-ddad-11da-8ad9-0800200c9a66
##1abe70e9-ddad-11da-8ad9-0800200c9a66
##1abe70ea-ddad-11da-8ad9-0800200c9a66
##1abe70eb-ddad-11da-8ad9-0800200c9a66
##1abe70ec-ddad-11da-8ad9-0800200c9a66
##1abe70ed-ddad-11da-8ad9-0800200c9a66
##1abe70ee-ddad-11da-8ad9-0800200c9a66
##1abe97f0-ddad-11da-8ad9-0800200c9a660

