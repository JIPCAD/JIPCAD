grammar Nom;

file : command* EOF;

expression
   :  ident LPAREN expression RPAREN # Call
   |  op= (PLUS | MINUS) expression # UnaryOp
   |  expression op=POW expression # BinOp
   |  expression op=(TIMES | DIV)  expression # BinOp
   |  expression op=(PLUS | MINUS) expression # BinOp
   |  LPAREN expression RPAREN # SubExpParen
   |  beg='{' sec=EXPR expression end='}' # SubExpCurly
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
   : id
   | '$' id
   ;

id
   : IDENT
   | EXPR
	| CLOSED
	| SD_TYPE
	| SD_LEVEL
	| OFFSET_TYPE
	| HEIGHT
	| WIDTH
	| HIDDENC
	| BEGINCAP
	| ENDCAP
	| SURFACE
	| BACKFACE
	| SEGS
	| ORDER
	| TYPE
	| COLOR
	| DIRECTION
	| PROJECTION
	| CAMERAID
	| FRUSTUM
	| ROTATE
	| SCALE
	| TRANSLATE
	| CROTATE
	| CTRANSLATE
	| ROTATE
	| SCALE
	| POINT
	| AZIMUTH
	| TWIST
	| REVERSE
	| MINTORSION
	| ORIGIN
	| SIZE
	| BACKGROUND
	| FUNC
	| FUNCX
	| FUNCY
	| FUNCZ
	| BOTCAP
	| TOPCAP
	| CUTBEGIN
	| CUTEND
	| MORPHINDEX
	| POINT
	| ENDPOINT
	| POLYLINE
	| ENDPOLYLINE
	| SWEEP
	| CROSSSECTION
	| ENDCROSSSECTION
	| PATH
	| ENDPATH
	| ENDSWEEP
	| SWEEPMORPH
	| ENDSWEEPMORPH
	| MORPHVISUALIZER
	| ENDMORPHVISUALIZER
	| FACE
	| ENDFACE
	| CONTROLPOINT
	| ENDCONTROLPOINT
	| OBJECT
	| ENDOBJECT
	| MESH
	| ENDMESH
	| GROUP
	| ENDGROUP
	| CIRCLE
	| ENDCIRCLE
	| SPIRAL
	| ENDSPIRAL
	| SPHERE
	| ENDSPHERE
	| ELLIPSOID
	| ENDELLIPSOID
	| CYLINDER
	| ENDCYLINDER
	| HYPERBOLOID
	| ENDHYPERBOLOID
	| DUPIN
	| ENDDUPIN
	| MOBIUSSTRIP
	| ENDMOBIUSSTRIP
	| HELIX
	| ENDHELIX
	| FUNNEL
	| ENDFUNNEL
	| TUNNEL
	| ENDTUNNEL
	| TORUSKNOT
	| ENDTORUSKNOT
	| TORUS
	| ENDTORUS
	| GENCARTESIANSURF
	| ENDGENCARTESIANSURF
	| GENPARAMETRICSURF
	| ENDGENPARAMETRICSURF
	| GENIMPLICITSURF
	| ENDGENIMPLICITSURF
	| BEZIERCURVE
	| ENDBEZIERCURVE
	| BSPLINE
	| ENDBSPLINE
	| INSTANCE
	| ENDINSTANCE
	| SURFACE
	| ENDSURFACE
	| FRONTCOLOR
	| ENDFRONTCOLOR
	| BACKCOLOR
	| ENDBACKCOLOR
	| BACKGROUNDCOLOR
	| ENDBACKGROUNDCOLOR
	| BACKFACE
	| ENDBACKFACE
	| WINDOW
	| ENDWINDOW
	| FOREGROUND
	| ENDFOREGROUND
	| INSIDEFACES
	| ENDINSIDEFACES
	| OUTSIDEFACES
	| ENDOUTSIDEFACES
	| OFFSETFACES
	| ENDOFFSETFACES
	| FRONTFACES
	| ENDFRONTFACES
	| BACKFACES
	| ENDBACKFACES
	| RIMFACES
	| ENDRIMFACES
	| BANK
	| ENDBANK
	| DELETE
	| ENDDELETE
	| SUBDIVISION
	| ENDSUBDIVISION
	| SHARP
	| ENDSHARP
	| OFFSET
	| ENDOFFSET
	| INCLUDE
	| ENDINCLUDE
	| LIGHT
	| ENDLIGHT
	| CAMERA
	| ENDCAMERA
	| VIEWPORT
	| ENDVIEWPORT
	| SET
	| LIST
	| FACE
	| ENDFACE;

idList : LPAREN (identList+=ident)* RPAREN ;
vector3 : LPAREN expression expression expression RPAREN ;


argClosed : CLOSED ;
argSdFlag : SD_TYPE ident;
argSdLevel : SD_LEVEL expression;
argOffsetFlag : OFFSET_TYPE ident;
argHeight : HEIGHT expression;
argWidth : WIDTH expression;
argHidden : HIDDENC ;
argBeginCap : BEGINCAP ;
argEndCap : ENDCAP ;
argSurface : SURFACE ident ;
argBackface : BACKFACE ident ;
argCross : CROSSSECTION ident ;
argSegs : SEGS expression ;
argOrder : ORDER expression ;
argLightType : TYPE ident;
argLightColor : COLOR vector3;
argLightVector : DIRECTION vector3;
argCameraProjection : PROJECTION ident;
argCameraID : CAMERAID ident;
argCameraFrustum : FRUSTUM LPAREN exp1=expression exp2=expression exp3=expression exp4=expression RPAREN LPAREN exp5=expression exp6=expression RPAREN;
argTransform
   : ROTATE LPAREN exp1=expression exp2=expression exp3=expression RPAREN LPAREN exp4=expression RPAREN # argTransformTwo
   | SCALE LPAREN expression expression expression RPAREN # argTransformOne
   | TRANSLATE LPAREN expression expression expression RPAREN # argTransformOne
   ;

argRotate : CROTATE LPAREN exp1=expression exp2=expression exp3=expression RPAREN LPAREN exp4=expression RPAREN;
argTranslate : CTRANSLATE LPAREN expression expression expression RPAREN;
argColor : COLOR LPAREN expression expression expression RPAREN ;
argControlRotate : ROTATE vector3 ;
argControlScale : SCALE vector3 ;
argPoint : POINT ident ;
argAzimuth : AZIMUTH expression ;
argTwist : TWIST expression ;
argReverse : REVERSE ;
argMintorsion : MINTORSION ;
argOrigin : ORIGIN LPAREN expression expression RPAREN;
argSize : SIZE LPAREN expression expression RPAREN;
argBackground : BACKGROUND LPAREN expression expression expression RPAREN;
argFunc : FUNC ident ;
argFuncX : FUNCX ident ;
argFuncY : FUNCY ident ;
argFuncZ : FUNCZ ident ;
argBotCap : BOTCAP ;
argTopCap : TOPCAP ;
argCutBegin : CUTBEGIN ;
argCutEnd : CUTEND ;
argMorphIndex : MORPHINDEX expression ;

command
   : open=POINT name=ident LPAREN expression expression expression RPAREN end=ENDPOINT # CmdExprListOne
   | open=POLYLINE name=ident idList argClosed* end=ENDPOLYLINE # CmdIdListOne
   | open=SWEEP name=ident CROSSSECTION crossId=ident (argBeginCap | argEndCap | argReverse)* ENDCROSSSECTION PATH pathId=ident (argAzimuth | argTwist | argMintorsion | argCutBegin | argCutEnd)* ENDPATH end=ENDSWEEP # CmdSweep
   | open=SWEEPMORPH name=ident (argBeginCap | argEndCap | argReverse)* PATH pathId=ident (argAzimuth | argTwist | argMintorsion | argCutBegin | argCutEnd)* ENDPATH  end=ENDSWEEPMORPH # CmdSweepMorph
   | open=MORPHVISUALIZER name=ident SWEEP morphId=ident argMorphIndex end=ENDMORPHVISUALIZER # CmdMorphVisualizer
   | open=FACE name=ident idList argSurface* argBackface* end=ENDFACE # CmdIdListOne
   | open=CONTROLPOINT name=ident argPoint (argControlScale | argControlRotate | argCross | argReverse)* end=ENDCONTROLPOINT # CmdNamedArgs
   | open=OBJECT name=ident idList end=ENDOBJECT # CmdIdListOne
   | open=MESH name=ident command* end=ENDMESH # CmdSubCmds
   | open=GROUP name=ident command* end=ENDGROUP # CmdSubCmds
   | open=CIRCLE name=ident LPAREN expression expression RPAREN end=ENDCIRCLE # CmdExprListOne
   | open=SPIRAL name=ident LPAREN expression expression expression RPAREN end=ENDSPIRAL # CmdExprListOne
   | open=SPHERE name=ident LPAREN expression expression expression expression expression expression RPAREN end=ENDSPHERE # CmdExprListOne
   | open=ELLIPSOID name=ident LPAREN expression expression expression expression expression expression expression RPAREN end=ENDELLIPSOID # CmdExprListOne
   | open=CYLINDER name=ident LPAREN expression expression expression expression (argBotCap | argTopCap)* RPAREN end=ENDCYLINDER # CmdExprListOne
   | open=HYPERBOLOID name=ident LPAREN expression expression expression expression expression expression RPAREN end=ENDHYPERBOLOID # CmdExprListOne
   | open=DUPIN name=ident LPAREN expression expression expression expression expression expression expression RPAREN end=ENDDUPIN # CmdExprListOne
   | open=MOBIUSSTRIP name=ident LPAREN expression expression expression expression RPAREN end=ENDMOBIUSSTRIP # CmdExprListOne
   | open=HELIX name=ident LPAREN expression expression expression expression RPAREN end=ENDHELIX # CmdExprListOne
   | open=FUNNEL name=ident LPAREN expression expression expression expression RPAREN end=ENDFUNNEL # CmdExprListOne
   | open=TUNNEL name=ident LPAREN expression expression expression expression RPAREN end=ENDTUNNEL # CmdExprListOne
   | open=TORUSKNOT name=ident LPAREN expression expression expression expression expression expression expression RPAREN end=ENDTORUSKNOT # CmdExprListOne
   | open=TORUS name=ident LPAREN expression expression expression expression expression expression expression RPAREN end=ENDTORUS # CmdExprListOne
   | open=GENCARTESIANSURF name=ident argFunc LPAREN expression expression expression expression expression expression RPAREN end=ENDGENCARTESIANSURF # CmdGeneral
   | open=GENPARAMETRICSURF name=ident argFuncX argFuncY argFuncZ LPAREN expression expression expression expression expression expression RPAREN end=ENDGENPARAMETRICSURF # CmdGeneralParametric
   | open=GENIMPLICITSURF name=ident argFunc LPAREN expression expression expression expression expression expression expression expression expression RPAREN end=ENDGENIMPLICITSURF # CmdGeneral
   | open=BEZIERCURVE name=ident idList argSegs* end=ENDBEZIERCURVE # CmdIdListOne
   | open=BSPLINE name=ident argOrder* idList argSegs* end=ENDBSPLINE # CmdIdListOne
   | open=INSTANCE name=ident entity=ident (argSurface | argTransform | argHidden | argBackface)* end=ENDINSTANCE # CmdInstance
   | open=SURFACE name=ident argColor end=ENDSURFACE # CmdSurface
   | open=FRONTCOLOR LPAREN expression expression expression RPAREN end=ENDFRONTCOLOR # CmdInitColor
   | open=BACKCOLOR LPAREN expression expression expression RPAREN end=ENDBACKCOLOR # CmdInitColor
   | open=BACKGROUNDCOLOR LPAREN expression expression expression RPAREN end=ENDBACKGROUNDCOLOR # CmdInitColor
   | open=BACKFACE name=ident argColor end=ENDBACKFACE # CmdSurface
   | open=WINDOW name=ident argOrigin argSize argBackground end=ENDWINDOW # CmdWindow
   | open=FOREGROUND argSurface end=ENDFOREGROUND # CmdArgSurface
   | open=INSIDEFACES argSurface end=ENDINSIDEFACES # CmdArgSurface
   | open=OUTSIDEFACES argSurface end=ENDOUTSIDEFACES # CmdArgSurface
   | open=OFFSETFACES argSurface end=ENDOFFSETFACES # CmdArgSurface
   | open=FRONTFACES argSurface end=ENDFRONTFACES # CmdArgSurface
   | open=BACKFACES argSurface end=ENDBACKFACES # CmdArgSurface
   | open=RIMFACES argSurface end=ENDRIMFACES # CmdArgSurface
   | open=BANK name=ident set* list end=ENDBANK # CmdBank
   | open=DELETE deleteFace* end=ENDDELETE # CmdDelete
   | open=SUBDIVISION name=ident argSdFlag* argSdLevel* command* end=ENDSUBDIVISION # CmdSubdivision
   | open=SHARP expression idList+ end=ENDSHARP # CmdSharp
   | open=OFFSET name=ident argOffsetFlag* argHeight* argWidth* command* end=ENDOFFSET # CmdOffset
   | open=INCLUDE name=ident end=ENDINCLUDE # CmdInclude
   | open=LIGHT name=ident argLightType argLightColor argLightVector* end=ENDLIGHT # CmdLight
   | open=CAMERA name=ident argCameraProjection (argCameraFrustum)* end=ENDCAMERA #CmdCamera
   | open=VIEWPORT name=ident argCameraID argOrigin argSize end=ENDVIEWPORT #CmdViewport
   ;


set : open=SET ident expression expression expression expression;
list: open=LIST idList;

deleteFace : open=FACE ident end=ENDFACE ;






//TOKENIZER

EXPR : 'expr';
CLOSED : 'closed';
SD_TYPE : 'sd_type';
SD_LEVEL : 'sd_level';
OFFSET_TYPE : 'offset_type';
HEIGHT : 'height';
WIDTH : 'width';
HIDDENC : 'hidden';
BEGINCAP : 'begincap';
ENDCAP : 'endcap';
SURFACE : 'surface';
BACKFACE : 'backface';
SEGS : 'segs';
ORDER : 'order';
TYPE : 'type';
COLOR : 'color';
DIRECTION : 'direction'; 
PROJECTION : 'projection';
CAMERAID : 'cameraID';
FRUSTUM : 'frustum';
ROTATE : 'rotate';
SCALE : 'scale';
TRANSLATE : 'translate';
CROTATE : 'crotate';
CTRANSLATE : 'ctranslate';
POINT : 'point';
AZIMUTH : 'azimuth';
TWIST : 'twist';
REVERSE : 'reverse';
MINTORSION : 'mintorsion';
ORIGIN : 'origin';
SIZE : 'size';
BACKGROUND : 'background';
FUNC : 'func';
FUNCX : 'funcX';
FUNCY : 'funcY';
FUNCZ : 'funcZ';
BOTCAP : 'botcap';
TOPCAP : 'topcap';
CUTBEGIN : 'cutbegin';
CUTEND : 'cutend';
MORPHINDEX : 'index';
ENDPOINT : 'endpoint';
POLYLINE : 'polyline';
ENDPOLYLINE : 'endpolyline';
SWEEP : 'sweep';
CROSSSECTION : 'crosssection';
ENDCROSSSECTION : 'endcrosssection';
PATH : 'path';
ENDPATH : 'endpath';
ENDSWEEP : 'endsweep';
SWEEPMORPH : 'sweepmorph';
ENDSWEEPMORPH : 'endsweepmorph';
MORPHVISUALIZER : 'morphvisualizer';
ENDMORPHVISUALIZER : 'endmorphvisualizer';
ENDFACE : 'endface';
CONTROLPOINT : 'controlpoint';
ENDCONTROLPOINT : 'endcontrolpoint';
OBJECT : 'object';
ENDOBJECT : 'endobject';
MESH : 'mesh';
ENDMESH : 'endmesh';
GROUP : 'group';
ENDGROUP : 'endgroup';
CIRCLE : 'circle';
ENDCIRCLE : 'endcircle';
SPIRAL : 'spiral';
ENDSPIRAL : 'endspiral';
SPHERE : 'sphere';
ENDSPHERE : 'endsphere';
ELLIPSOID : 'ellipsoid';
ENDELLIPSOID : 'endellipsoid';
CYLINDER : 'cylinder';
ENDCYLINDER : 'endcylinder';
HYPERBOLOID : 'hyperboloid';
ENDHYPERBOLOID : 'endhyperboloid';
DUPIN : 'dupin';
ENDDUPIN : 'enddupin';
MOBIUSSTRIP : 'mobiusstrip';
ENDMOBIUSSTRIP : 'endmobiusstrip';
HELIX : 'helix';
ENDHELIX : 'endhelix';
FUNNEL : 'funnel';
ENDFUNNEL : 'endfunnel';
TUNNEL : 'tunnel';
ENDTUNNEL : 'endtunnel';
TORUSKNOT : 'torusknot';
ENDTORUSKNOT : 'endtorusknot';
TORUS : 'torus';
ENDTORUS : 'endtorus';
GENCARTESIANSURF : 'gencartesiansurf';
ENDGENCARTESIANSURF : 'endgencartesiansurf';
GENPARAMETRICSURF : 'genparametricsurf';
ENDGENPARAMETRICSURF : 'endgenparametricsurf';
GENIMPLICITSURF : 'genimplicitsurf';
ENDGENIMPLICITSURF : 'endgenimplicitsurf';
BEZIERCURVE : 'beziercurve';
ENDBEZIERCURVE : 'endbeziercurve';
BSPLINE : 'bspline';
ENDBSPLINE : 'endbspline';
INSTANCE : 'instance';
ENDINSTANCE : 'endinstance';
ENDSURFACE : 'endsurface';
FRONTCOLOR : 'frontcolor';
ENDFRONTCOLOR : 'endfrontcolor';
BACKCOLOR : 'backcolor';
ENDBACKCOLOR : 'endbackcolor';
BACKGROUNDCOLOR: 'backgroundcolor';
ENDBACKGROUNDCOLOR : 'endbackgroundcolor';
ENDBACKFACE : 'endbackface';
WINDOW : 'window';
ENDWINDOW : 'endwindow';
FOREGROUND : 'foreground';
ENDFOREGROUND : 'endforeground';
INSIDEFACES : 'insidefaces';
ENDINSIDEFACES : 'endinsidefaces';
OUTSIDEFACES : 'outsidefaces';
ENDOUTSIDEFACES : 'endoutsidefaces';
OFFSETFACES : 'offsetfaces';
ENDOFFSETFACES : 'endoffsetfaces';
FRONTFACES : 'frontfaces';
ENDFRONTFACES : 'endfrontfaces';
BACKFACES : 'backfaces';
ENDBACKFACES : 'endbackfaces';
RIMFACES : 'rimfaces';
ENDRIMFACES : 'endrimfaces';
BANK : 'bank';
ENDBANK : 'endbank';
DELETE : 'delete';
ENDDELETE : 'enddelete';
SUBDIVISION : 'subdivision';
ENDSUBDIVISION : 'endsubdivision';
SHARP : 'sharp';
ENDSHARP : 'endsharp';
OFFSET : 'offset';
ENDOFFSET : 'endoffset';
INCLUDE : 'include';
ENDINCLUDE : 'endinclude';
LIGHT : 'light';
ENDLIGHT : 'endlight';
CAMERA : 'camera';
ENDCAMERA : 'endcamera';
VIEWPORT : 'viewport';
ENDVIEWPORT : 'endviewport';
SET : 'set';
LIST : 'list';
FACE : 'face';


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
DOT : '.' ;
POW : '^' ;
WS : [ \r\n\t] + -> skip ;

COMMENT : '(*' .*? '*)' -> skip ;
LINE_COMMENT : '#' ~('\n'|'\r')* -> skip ;
