grammar Nom;

file : command* EOF;

expression
   :  ident LPAREN expression RPAREN # Call
   |  op= (PLUS | MINUS) expression # UnaryOp
   |  expression op=POW expression # BinOp
   |  expression op=(TIMES | DIV)  expression # BinOp
   |  expression op=(PLUS | MINUS) expression # BinOp
   |  LPAREN expression RPAREN # SubExpParen
   |  beg='{' sec='expr' expression end='}' # SubExpCurly
   |  atom # AtomExpr
   ;


atom
   : scientific
   | ident
   ;

scientific
   : SCIENTIFIC_NUMBER
   ;

ident
   : IDENT
   | '$' IDENT
   ;

idList : LPAREN (identList+=ident)* RPAREN ;
vector3 : LPAREN expression expression expression RPAREN ;


argClosed : 'closed' ;
argSdFlag : 'sd_type' ident;
argSdLevel : 'sd_level' expression;
argOffsetFlag : 'offset_type' ident;
argHeight : 'height' expression;
argWidth : 'width' expression;
argHidden : 'hidden' ;
argBeginCap : 'begincap' ;
argEndCap : 'endcap' ;
argSurface : 'surface' ident ;
argCross : 'cross' ident ;
argSegs : 'segs' expression ;
argOrder : 'order' expression ;
argLightType : 'type' ident;
argLightColor : 'color' vector3;
argCameraProjection : 'projection' ident;
argCameraID : 'cameraID' ident;
argCameraFrustum : 'frustum' LPAREN expression* RPAREN;
argTransform
   : 'rotate' LPAREN exp1=expression exp2=expression exp3=expression RPAREN LPAREN exp4=expression RPAREN # argTransformTwo
   | 'scale' LPAREN expression expression expression RPAREN # argTransformOne
   | 'translate' LPAREN expression expression expression RPAREN # argTransformOne
   ;
argRotate : 'crotate' LPAREN exp1=expression exp2=expression exp3=expression RPAREN LPAREN exp4=expression RPAREN;
argTranslate : 'ctranslate' LPAREN expression expression expression RPAREN;

argColor : 'color' LPAREN expression expression expression RPAREN ;
argControlRotate : 'rotate' vector3 ;
argControlScale : 'scale' vector3 ;
argPoint : 'point' ident ;
argAzimuth : 'azimuth' expression ;
argTwist : 'twist' expression ;
argReverse : 'reverse' ;
argMintorsion : 'mintorsion' ;
argFunc : 'func' ident ;
argFuncX : 'funcX' ident ;
argFuncY : 'funcY' ident ;
argFuncZ : 'funcZ' ident ;

command
   : open='point' name=ident LPAREN expression expression expression RPAREN end='endpoint' # CmdExprListOne
   | open='polyline' name=ident idList argClosed* end='endpolyline' # CmdIdListOne
   | open='sweep' name=ident 'crosssection' crossId=ident (argBeginCap | argEndCap | argReverse)* 'endcrosssection' 'path' pathId=ident (argAzimuth | argTwist | argMintorsion)* 'endpath'  end='endsweep' # CmdSweep
   | open='controlpoint' name=ident argPoint argControlScale argControlRotate (argCross | argReverse)* end='endcontrolpoint' # CmdNamedArgs
   | open='face' name=ident idList argSurface* end='endface' # CmdIdListOne
   | open='object' name=ident idList end='endobject' # CmdIdListOne
   | open='mesh' name=ident command* end='endmesh' # CmdSubCmds
   | open='group' name=ident command* end='endgroup' # CmdSubCmds
   | open='circle' name=ident LPAREN expression expression RPAREN end='endcircle' # CmdExprListOne
   | open='spiral' name=ident LPAREN expression expression expression RPAREN end='endspiral' # CmdExprListOne
   | open='sphere' name=ident LPAREN expression expression expression expression expression expression RPAREN end='endsphere' # CmdExprListOne
   | open='ellipsoid' name=ident LPAREN expression expression expression expression expression expression expression RPAREN end='endellipsoid' # CmdExprListOne
   | open='cylinder' name=ident LPAREN expression expression expression expression RPAREN end='endcylinder' # CmdExprListOne
   | open='hyperboloid' name=ident LPAREN expression expression expression expression expression expression RPAREN end='endhyperboloid' # CmdExprListOne
   | open='dupin' name=ident LPAREN expression expression expression expression expression expression expression RPAREN end='enddupin' # CmdExprListOne
   | open='mobiusstrip' name=ident LPAREN expression expression expression expression RPAREN end='endmobiusstrip' # CmdExprListOne
   | open='helix' name=ident LPAREN expression expression expression expression RPAREN end='endhelix' # CmdExprListOne
   | open='funnel' name=ident LPAREN expression expression expression expression RPAREN end='endfunnel' # CmdExprListOne
   | open='tunnel' name=ident LPAREN expression expression expression expression RPAREN end='endtunnel' # CmdExprListOne
   | open='torusknot' name=ident LPAREN expression expression expression expression expression expression expression RPAREN end='endtorusknot' # CmdExprListOne
   | open='torus' name=ident LPAREN expression expression expression expression expression expression expression RPAREN end='endtorus' # CmdExprListOne
   | open='gencartesiansurf' name=ident argFunc LPAREN expression expression expression expression expression expression RPAREN end='endgencartesiansurf' # CmdGeneral
   | open='genparametricsurf' name=ident argFuncX argFuncY argFuncZ LPAREN expression expression expression expression expression expression RPAREN end='endgenparametricsurf' # CmdGeneralParametric
   | open='genimplicitsurf' name=ident argFunc LPAREN expression expression expression expression expression expression expression expression expression RPAREN end='endgenimplicitsurf' # CmdGeneral
   | open='beziercurve' name=ident idList argSegs* end='endbeziercurve' # CmdIdListOne
   | open='bspline' name=ident argOrder* idList argSegs* end='endbspline' # CmdIdListOne
   | open='instance' name=ident entity=ident (argSurface | argTransform | argHidden)* end='endinstance' # CmdInstance
   | open='surface' name=ident argColor end='endsurface' # CmdSurface
   | open='background' name=ident LPAREN expression expression expression RPAREN end='endbackground' # CmdExprListOne
   | open='foreground' argSurface end='endforeground' # CmdArgSurface
   | open='insidefaces' argSurface end='endinsidefaces' # CmdArgSurface
   | open='outsidefaces' argSurface end='endoutsidefaces' # CmdArgSurface
   | open='offsetfaces' argSurface end='endoffsetfaces' # CmdArgSurface
   | open='frontfaces' argSurface end='endfrontfaces' # CmdArgSurface
   | open='backfaces' argSurface end='endbackfaces' # CmdArgSurface
   | open='rimfaces' argSurface end='endrimfaces' # CmdArgSurface
   | open='bank' name=ident set* end='endbank' # CmdBank
   | open='delete' deleteFace* end='enddelete' # CmdDelete
   | open='subdivision' name=ident argSdFlag* argSdLevel* command* end='endsubdivision' # CmdSubdivision
   | open='sharp' expression idList+ end='endsharp' # CmdSharp
   | open='offset' name=ident argOffsetFlag* argHeight* argWidth* command* end='endoffset' # CmdOffset
   | open='include' name=ident end='endinclude' # CmdInclude
   | open='light' name=ident argLightType argLightColor end='endlight' # CmdLight
   | open='camera' name=ident argCameraProjection argCameraFrustum argTranslate* argRotate* end='endcamera' #CmdCamera
   | open='viewport' name=ident argCameraID LPAREN expression expression expression expression RPAREN end='endviewport' #CmdViewport
   ;


set : open='set' ident expression expression expression expression;

deleteFace : open='face' ident end='endface' ;

IDENT : VALID_ID_START VALID_ID_CHAR* | OPENBRACKET VALID_ID_FUNC* CLOSEBRACKET ;
fragment VALID_ID_START : ('a' .. 'z') | ('A' .. 'Z') | '_' | '.' ;
fragment VALID_ID_CHAR : VALID_ID_START | ('0' .. '9') ;
fragment VALID_ID_FUNC : VALID_ID_CHAR | '(' | ')' | '*' | '/' | '+' | '!' | '%' | '=' | '^' | '-' | '|' | ',' | '<' | '>' ;
fragment OPENBRACKET : '[' ;
fragment CLOSEBRACKET : ']' ;

//The NUMBER part gets its potential sign from "(PLUS | MINUS)* atom" in the expression rule
SCIENTIFIC_NUMBER : NUMBER (E SIGN? UNSIGNED_INTEGER)? ;
fragment NUMBER : ('0' .. '9') + ('.' ('0' .. '9') +)? ;
fragment E : 'E' | 'e' ;
fragment SIGN : ('+' | '-') ;
fragment UNSIGNED_INTEGER : ('0' .. '9')+ ;


LPAREN : '(' ;
RPAREN : ')' ;
PLUS : '+' ;
MINUS : '-' ;
TIMES : '*' ;
DIV : '/' ;
GT : '>' ;
LT : '<' ;
EQ : '=' ;
POINT : '.' ;
POW : '^' ;
WS : [ \r\n\t] + -> skip ;

COMMENT : '(*' .*? '*)' -> skip ;
LINE_COMMENT : '#' ~('\n'|'\r')* -> skip ;
