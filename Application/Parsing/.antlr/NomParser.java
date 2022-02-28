// Generated from /Users/samuelspeas/JIPCADC/JIPCAD/Application/Parsing/Nom.g4 by ANTLR 4.8
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class NomParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.8", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, T__12=13, T__13=14, T__14=15, T__15=16, T__16=17, 
		T__17=18, T__18=19, T__19=20, T__20=21, T__21=22, T__22=23, T__23=24, 
		T__24=25, T__25=26, T__26=27, T__27=28, T__28=29, T__29=30, T__30=31, 
		T__31=32, T__32=33, T__33=34, T__34=35, T__35=36, T__36=37, T__37=38, 
		T__38=39, T__39=40, T__40=41, T__41=42, T__42=43, T__43=44, T__44=45, 
		T__45=46, T__46=47, T__47=48, T__48=49, T__49=50, T__50=51, T__51=52, 
		T__52=53, T__53=54, T__54=55, T__55=56, T__56=57, T__57=58, T__58=59, 
		T__59=60, T__60=61, T__61=62, T__62=63, T__63=64, T__64=65, T__65=66, 
		T__66=67, T__67=68, T__68=69, T__69=70, T__70=71, T__71=72, T__72=73, 
		T__73=74, T__74=75, T__75=76, T__76=77, T__77=78, T__78=79, T__79=80, 
		T__80=81, T__81=82, T__82=83, T__83=84, T__84=85, T__85=86, T__86=87, 
		T__87=88, T__88=89, T__89=90, T__90=91, T__91=92, T__92=93, T__93=94, 
		T__94=95, T__95=96, T__96=97, T__97=98, T__98=99, T__99=100, T__100=101, 
		T__101=102, T__102=103, T__103=104, T__104=105, T__105=106, T__106=107, 
		T__107=108, T__108=109, T__109=110, T__110=111, T__111=112, T__112=113, 
		T__113=114, T__114=115, T__115=116, T__116=117, T__117=118, T__118=119, 
		T__119=120, T__120=121, T__121=122, T__122=123, T__123=124, T__124=125, 
		T__125=126, T__126=127, T__127=128, T__128=129, T__129=130, T__130=131, 
		T__131=132, T__132=133, T__133=134, T__134=135, T__135=136, T__136=137, 
		T__137=138, T__138=139, T__139=140, T__140=141, T__141=142, T__142=143, 
		T__143=144, IDENT=145, SCIENTIFIC_NUMBER=146, LPAREN=147, RPAREN=148, 
		PLUS=149, MINUS=150, TIMES=151, DIV=152, GT=153, LT=154, EQ=155, POINT=156, 
		POW=157, WS=158, COMMENT=159, LINE_COMMENT=160;
	public static final int
		RULE_file = 0, RULE_expression = 1, RULE_atom = 2, RULE_scientific = 3, 
		RULE_ident = 4, RULE_idList = 5, RULE_vector3 = 6, RULE_argClosed = 7, 
		RULE_argSdFlag = 8, RULE_argSdLevel = 9, RULE_argOffsetFlag = 10, RULE_argHeight = 11, 
		RULE_argWidth = 12, RULE_argHidden = 13, RULE_argBeginCap = 14, RULE_argEndCap = 15, 
		RULE_argSurface = 16, RULE_argBackface = 17, RULE_argCross = 18, RULE_argSegs = 19, 
		RULE_argOrder = 20, RULE_argLightType = 21, RULE_argLightColor = 22, RULE_argCameraProjection = 23, 
		RULE_argCameraID = 24, RULE_argCameraFrustum = 25, RULE_argTransform = 26, 
		RULE_argRotate = 27, RULE_argTranslate = 28, RULE_argColor = 29, RULE_argControlRotate = 30, 
		RULE_argControlScale = 31, RULE_argPoint = 32, RULE_argAzimuth = 33, RULE_argTwist = 34, 
		RULE_argReverse = 35, RULE_argMintorsion = 36, RULE_argOrigin = 37, RULE_argSize = 38, 
		RULE_argBackground = 39, RULE_argFunc = 40, RULE_argFuncX = 41, RULE_argFuncY = 42, 
		RULE_argFuncZ = 43, RULE_argBotCap = 44, RULE_argTopCap = 45, RULE_argCutBegin = 46, 
		RULE_argCutEnd = 47, RULE_argJoin = 48, RULE_command = 49, RULE_set = 50, 
		RULE_list = 51, RULE_deleteFace = 52;
	private static String[] makeRuleNames() {
		return new String[] {
			"file", "expression", "atom", "scientific", "ident", "idList", "vector3", 
			"argClosed", "argSdFlag", "argSdLevel", "argOffsetFlag", "argHeight", 
			"argWidth", "argHidden", "argBeginCap", "argEndCap", "argSurface", "argBackface", 
			"argCross", "argSegs", "argOrder", "argLightType", "argLightColor", "argCameraProjection", 
			"argCameraID", "argCameraFrustum", "argTransform", "argRotate", "argTranslate", 
			"argColor", "argControlRotate", "argControlScale", "argPoint", "argAzimuth", 
			"argTwist", "argReverse", "argMintorsion", "argOrigin", "argSize", "argBackground", 
			"argFunc", "argFuncX", "argFuncY", "argFuncZ", "argBotCap", "argTopCap", 
			"argCutBegin", "argCutEnd", "argJoin", "command", "set", "list", "deleteFace"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'{'", "'expr'", "'}'", "'$'", "'closed'", "'sd_type'", "'sd_level'", 
			"'offset_type'", "'height'", "'width'", "'hidden'", "'begincap'", "'endcap'", 
			"'surface'", "'backface'", "'cross'", "'segs'", "'order'", "'type'", 
			"'color'", "'projection'", "'cameraID'", "'frustum'", "'rotate'", "'scale'", 
			"'translate'", "'crotate'", "'ctranslate'", "'point'", "'azimuth'", "'twist'", 
			"'reverse'", "'mintorsion'", "'origin'", "'size'", "'background'", "'func'", 
			"'funcX'", "'funcY'", "'funcZ'", "'botcap'", "'topcap'", "'cutbegin'", 
			"'cutend'", "'join'", "'endpoint'", "'polyline'", "'endpolyline'", "'sweep'", 
			"'crosssection'", "'endcrosssection'", "'path'", "'endpath'", "'endsweep'", 
			"'face'", "'endface'", "'controlpoint'", "'endcontrolpoint'", "'object'", 
			"'endobject'", "'mesh'", "'endmesh'", "'group'", "'endgroup'", "'circle'", 
			"'endcircle'", "'spiral'", "'endspiral'", "'sphere'", "'endsphere'", 
			"'ellipsoid'", "'endellipsoid'", "'cylinder'", "'endcylinder'", "'hyperboloid'", 
			"'endhyperboloid'", "'dupin'", "'enddupin'", "'mobiusstrip'", "'endmobiusstrip'", 
			"'helix'", "'endhelix'", "'funnel'", "'endfunnel'", "'tunnel'", "'endtunnel'", 
			"'torusknot'", "'endtorusknot'", "'torus'", "'endtorus'", "'gencartesiansurf'", 
			"'endgencartesiansurf'", "'genparametricsurf'", "'endgenparametricsurf'", 
			"'genimplicitsurf'", "'endgenimplicitsurf'", "'beziercurve'", "'endbeziercurve'", 
			"'bspline'", "'endbspline'", "'instance'", "'endinstance'", "'endsurface'", 
			"'frontcolor'", "'endfrontcolor'", "'backcolor'", "'endbackcolor'", "'endbackface'", 
			"'window'", "'endwindow'", "'foreground'", "'endforeground'", "'insidefaces'", 
			"'endinsidefaces'", "'outsidefaces'", "'endoutsidefaces'", "'offsetfaces'", 
			"'endoffsetfaces'", "'frontfaces'", "'endfrontfaces'", "'backfaces'", 
			"'endbackfaces'", "'rimfaces'", "'endrimfaces'", "'bank'", "'endbank'", 
			"'delete'", "'enddelete'", "'subdivision'", "'endsubdivision'", "'sharp'", 
			"'endsharp'", "'offset'", "'endoffset'", "'include'", "'endinclude'", 
			"'light'", "'endlight'", "'camera'", "'endcamera'", "'viewport'", "'endviewport'", 
			"'set'", "'list'", null, null, "'('", "')'", "'+'", "'-'", "'*'", "'/'", 
			"'>'", "'<'", "'='", "'.'", "'^'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, "IDENT", "SCIENTIFIC_NUMBER", "LPAREN", "RPAREN", "PLUS", "MINUS", 
			"TIMES", "DIV", "GT", "LT", "EQ", "POINT", "POW", "WS", "COMMENT", "LINE_COMMENT"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "Nom.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public NomParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	public static class FileContext extends ParserRuleContext {
		public TerminalNode EOF() { return getToken(NomParser.EOF, 0); }
		public List<CommandContext> command() {
			return getRuleContexts(CommandContext.class);
		}
		public CommandContext command(int i) {
			return getRuleContext(CommandContext.class,i);
		}
		public FileContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_file; }
	}

	public final FileContext file() throws RecognitionException {
		FileContext _localctx = new FileContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_file);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(109);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (((((_la - 14)) & ~0x3f) == 0 && ((1L << (_la - 14)) & ((1L << (T__13 - 14)) | (1L << (T__14 - 14)) | (1L << (T__28 - 14)) | (1L << (T__46 - 14)) | (1L << (T__48 - 14)) | (1L << (T__54 - 14)) | (1L << (T__56 - 14)) | (1L << (T__58 - 14)) | (1L << (T__60 - 14)) | (1L << (T__62 - 14)) | (1L << (T__64 - 14)) | (1L << (T__66 - 14)) | (1L << (T__68 - 14)) | (1L << (T__70 - 14)) | (1L << (T__72 - 14)) | (1L << (T__74 - 14)) | (1L << (T__76 - 14)))) != 0) || ((((_la - 79)) & ~0x3f) == 0 && ((1L << (_la - 79)) & ((1L << (T__78 - 79)) | (1L << (T__80 - 79)) | (1L << (T__82 - 79)) | (1L << (T__84 - 79)) | (1L << (T__86 - 79)) | (1L << (T__88 - 79)) | (1L << (T__90 - 79)) | (1L << (T__92 - 79)) | (1L << (T__94 - 79)) | (1L << (T__96 - 79)) | (1L << (T__98 - 79)) | (1L << (T__100 - 79)) | (1L << (T__103 - 79)) | (1L << (T__105 - 79)) | (1L << (T__108 - 79)) | (1L << (T__110 - 79)) | (1L << (T__112 - 79)) | (1L << (T__114 - 79)) | (1L << (T__116 - 79)) | (1L << (T__118 - 79)) | (1L << (T__120 - 79)) | (1L << (T__122 - 79)) | (1L << (T__124 - 79)) | (1L << (T__126 - 79)) | (1L << (T__128 - 79)) | (1L << (T__130 - 79)) | (1L << (T__132 - 79)) | (1L << (T__134 - 79)) | (1L << (T__136 - 79)) | (1L << (T__138 - 79)) | (1L << (T__140 - 79)))) != 0)) {
				{
				{
				setState(106);
				command();
				}
				}
				setState(111);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(112);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ExpressionContext extends ParserRuleContext {
		public ExpressionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expression; }
	 
		public ExpressionContext() { }
		public void copyFrom(ExpressionContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class CallContext extends ExpressionContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public CallContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	public static class UnaryOpContext extends ExpressionContext {
		public Token op;
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public TerminalNode PLUS() { return getToken(NomParser.PLUS, 0); }
		public TerminalNode MINUS() { return getToken(NomParser.MINUS, 0); }
		public UnaryOpContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	public static class SubExpParenContext extends ExpressionContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public SubExpParenContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	public static class SubExpCurlyContext extends ExpressionContext {
		public Token beg;
		public Token sec;
		public Token end;
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public SubExpCurlyContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	public static class BinOpContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode POW() { return getToken(NomParser.POW, 0); }
		public TerminalNode TIMES() { return getToken(NomParser.TIMES, 0); }
		public TerminalNode DIV() { return getToken(NomParser.DIV, 0); }
		public TerminalNode PLUS() { return getToken(NomParser.PLUS, 0); }
		public TerminalNode MINUS() { return getToken(NomParser.MINUS, 0); }
		public BinOpContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	public static class AtomExprContext extends ExpressionContext {
		public AtomContext atom() {
			return getRuleContext(AtomContext.class,0);
		}
		public AtomExprContext(ExpressionContext ctx) { copyFrom(ctx); }
	}

	public final ExpressionContext expression() throws RecognitionException {
		return expression(0);
	}

	private ExpressionContext expression(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExpressionContext _localctx = new ExpressionContext(_ctx, _parentState);
		ExpressionContext _prevctx = _localctx;
		int _startState = 2;
		enterRecursionRule(_localctx, 2, RULE_expression, _p);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(132);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
			case 1:
				{
				_localctx = new CallContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;

				setState(115);
				ident();
				setState(116);
				match(LPAREN);
				setState(117);
				expression(0);
				setState(118);
				match(RPAREN);
				}
				break;
			case 2:
				{
				_localctx = new UnaryOpContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(120);
				((UnaryOpContext)_localctx).op = _input.LT(1);
				_la = _input.LA(1);
				if ( !(_la==PLUS || _la==MINUS) ) {
					((UnaryOpContext)_localctx).op = (Token)_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				setState(121);
				expression(7);
				}
				break;
			case 3:
				{
				_localctx = new SubExpParenContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(122);
				match(LPAREN);
				setState(123);
				expression(0);
				setState(124);
				match(RPAREN);
				}
				break;
			case 4:
				{
				_localctx = new SubExpCurlyContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(126);
				((SubExpCurlyContext)_localctx).beg = match(T__0);
				setState(127);
				((SubExpCurlyContext)_localctx).sec = match(T__1);
				setState(128);
				expression(0);
				setState(129);
				((SubExpCurlyContext)_localctx).end = match(T__2);
				}
				break;
			case 5:
				{
				_localctx = new AtomExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(131);
				atom();
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(145);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,3,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(143);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,2,_ctx) ) {
					case 1:
						{
						_localctx = new BinOpContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(134);
						if (!(precpred(_ctx, 6))) throw new FailedPredicateException(this, "precpred(_ctx, 6)");
						setState(135);
						((BinOpContext)_localctx).op = match(POW);
						setState(136);
						expression(7);
						}
						break;
					case 2:
						{
						_localctx = new BinOpContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(137);
						if (!(precpred(_ctx, 5))) throw new FailedPredicateException(this, "precpred(_ctx, 5)");
						setState(138);
						((BinOpContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==TIMES || _la==DIV) ) {
							((BinOpContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(139);
						expression(6);
						}
						break;
					case 3:
						{
						_localctx = new BinOpContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(140);
						if (!(precpred(_ctx, 4))) throw new FailedPredicateException(this, "precpred(_ctx, 4)");
						setState(141);
						((BinOpContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==PLUS || _la==MINUS) ) {
							((BinOpContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(142);
						expression(5);
						}
						break;
					}
					} 
				}
				setState(147);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,3,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public static class AtomContext extends ParserRuleContext {
		public ScientificContext scientific() {
			return getRuleContext(ScientificContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public AtomContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_atom; }
	}

	public final AtomContext atom() throws RecognitionException {
		AtomContext _localctx = new AtomContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_atom);
		try {
			setState(150);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case SCIENTIFIC_NUMBER:
				enterOuterAlt(_localctx, 1);
				{
				setState(148);
				scientific();
				}
				break;
			case T__3:
			case IDENT:
				enterOuterAlt(_localctx, 2);
				{
				setState(149);
				ident();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ScientificContext extends ParserRuleContext {
		public TerminalNode SCIENTIFIC_NUMBER() { return getToken(NomParser.SCIENTIFIC_NUMBER, 0); }
		public ScientificContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_scientific; }
	}

	public final ScientificContext scientific() throws RecognitionException {
		ScientificContext _localctx = new ScientificContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_scientific);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(152);
			match(SCIENTIFIC_NUMBER);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class IdentContext extends ParserRuleContext {
		public TerminalNode IDENT() { return getToken(NomParser.IDENT, 0); }
		public IdentContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_ident; }
	}

	public final IdentContext ident() throws RecognitionException {
		IdentContext _localctx = new IdentContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_ident);
		try {
			setState(157);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case IDENT:
				enterOuterAlt(_localctx, 1);
				{
				setState(154);
				match(IDENT);
				}
				break;
			case T__3:
				enterOuterAlt(_localctx, 2);
				{
				setState(155);
				match(T__3);
				setState(156);
				match(IDENT);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class IdListContext extends ParserRuleContext {
		public IdentContext ident;
		public List<IdentContext> identList = new ArrayList<IdentContext>();
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public List<IdentContext> ident() {
			return getRuleContexts(IdentContext.class);
		}
		public IdentContext ident(int i) {
			return getRuleContext(IdentContext.class,i);
		}
		public IdListContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_idList; }
	}

	public final IdListContext idList() throws RecognitionException {
		IdListContext _localctx = new IdListContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_idList);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(159);
			match(LPAREN);
			setState(163);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==T__3 || _la==IDENT) {
				{
				{
				setState(160);
				((IdListContext)_localctx).ident = ident();
				((IdListContext)_localctx).identList.add(((IdListContext)_localctx).ident);
				}
				}
				setState(165);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(166);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class Vector3Context extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public Vector3Context(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_vector3; }
	}

	public final Vector3Context vector3() throws RecognitionException {
		Vector3Context _localctx = new Vector3Context(_ctx, getState());
		enterRule(_localctx, 12, RULE_vector3);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(168);
			match(LPAREN);
			setState(169);
			expression(0);
			setState(170);
			expression(0);
			setState(171);
			expression(0);
			setState(172);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgClosedContext extends ParserRuleContext {
		public ArgClosedContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argClosed; }
	}

	public final ArgClosedContext argClosed() throws RecognitionException {
		ArgClosedContext _localctx = new ArgClosedContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_argClosed);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(174);
			match(T__4);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgSdFlagContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgSdFlagContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argSdFlag; }
	}

	public final ArgSdFlagContext argSdFlag() throws RecognitionException {
		ArgSdFlagContext _localctx = new ArgSdFlagContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_argSdFlag);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(176);
			match(T__5);
			setState(177);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgSdLevelContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgSdLevelContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argSdLevel; }
	}

	public final ArgSdLevelContext argSdLevel() throws RecognitionException {
		ArgSdLevelContext _localctx = new ArgSdLevelContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_argSdLevel);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(179);
			match(T__6);
			setState(180);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgOffsetFlagContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgOffsetFlagContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argOffsetFlag; }
	}

	public final ArgOffsetFlagContext argOffsetFlag() throws RecognitionException {
		ArgOffsetFlagContext _localctx = new ArgOffsetFlagContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_argOffsetFlag);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(182);
			match(T__7);
			setState(183);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgHeightContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgHeightContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argHeight; }
	}

	public final ArgHeightContext argHeight() throws RecognitionException {
		ArgHeightContext _localctx = new ArgHeightContext(_ctx, getState());
		enterRule(_localctx, 22, RULE_argHeight);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(185);
			match(T__8);
			setState(186);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgWidthContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgWidthContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argWidth; }
	}

	public final ArgWidthContext argWidth() throws RecognitionException {
		ArgWidthContext _localctx = new ArgWidthContext(_ctx, getState());
		enterRule(_localctx, 24, RULE_argWidth);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(188);
			match(T__9);
			setState(189);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgHiddenContext extends ParserRuleContext {
		public ArgHiddenContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argHidden; }
	}

	public final ArgHiddenContext argHidden() throws RecognitionException {
		ArgHiddenContext _localctx = new ArgHiddenContext(_ctx, getState());
		enterRule(_localctx, 26, RULE_argHidden);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(191);
			match(T__10);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgBeginCapContext extends ParserRuleContext {
		public ArgBeginCapContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argBeginCap; }
	}

	public final ArgBeginCapContext argBeginCap() throws RecognitionException {
		ArgBeginCapContext _localctx = new ArgBeginCapContext(_ctx, getState());
		enterRule(_localctx, 28, RULE_argBeginCap);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(193);
			match(T__11);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgEndCapContext extends ParserRuleContext {
		public ArgEndCapContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argEndCap; }
	}

	public final ArgEndCapContext argEndCap() throws RecognitionException {
		ArgEndCapContext _localctx = new ArgEndCapContext(_ctx, getState());
		enterRule(_localctx, 30, RULE_argEndCap);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(195);
			match(T__12);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgSurfaceContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgSurfaceContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argSurface; }
	}

	public final ArgSurfaceContext argSurface() throws RecognitionException {
		ArgSurfaceContext _localctx = new ArgSurfaceContext(_ctx, getState());
		enterRule(_localctx, 32, RULE_argSurface);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(197);
			match(T__13);
			setState(198);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgBackfaceContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgBackfaceContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argBackface; }
	}

	public final ArgBackfaceContext argBackface() throws RecognitionException {
		ArgBackfaceContext _localctx = new ArgBackfaceContext(_ctx, getState());
		enterRule(_localctx, 34, RULE_argBackface);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(200);
			match(T__14);
			setState(201);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCrossContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgCrossContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCross; }
	}

	public final ArgCrossContext argCross() throws RecognitionException {
		ArgCrossContext _localctx = new ArgCrossContext(_ctx, getState());
		enterRule(_localctx, 36, RULE_argCross);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(203);
			match(T__15);
			setState(204);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgSegsContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgSegsContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argSegs; }
	}

	public final ArgSegsContext argSegs() throws RecognitionException {
		ArgSegsContext _localctx = new ArgSegsContext(_ctx, getState());
		enterRule(_localctx, 38, RULE_argSegs);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(206);
			match(T__16);
			setState(207);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgOrderContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgOrderContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argOrder; }
	}

	public final ArgOrderContext argOrder() throws RecognitionException {
		ArgOrderContext _localctx = new ArgOrderContext(_ctx, getState());
		enterRule(_localctx, 40, RULE_argOrder);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(209);
			match(T__17);
			setState(210);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgLightTypeContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgLightTypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argLightType; }
	}

	public final ArgLightTypeContext argLightType() throws RecognitionException {
		ArgLightTypeContext _localctx = new ArgLightTypeContext(_ctx, getState());
		enterRule(_localctx, 42, RULE_argLightType);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(212);
			match(T__18);
			setState(213);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgLightColorContext extends ParserRuleContext {
		public Vector3Context vector3() {
			return getRuleContext(Vector3Context.class,0);
		}
		public ArgLightColorContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argLightColor; }
	}

	public final ArgLightColorContext argLightColor() throws RecognitionException {
		ArgLightColorContext _localctx = new ArgLightColorContext(_ctx, getState());
		enterRule(_localctx, 44, RULE_argLightColor);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(215);
			match(T__19);
			setState(216);
			vector3();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCameraProjectionContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgCameraProjectionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCameraProjection; }
	}

	public final ArgCameraProjectionContext argCameraProjection() throws RecognitionException {
		ArgCameraProjectionContext _localctx = new ArgCameraProjectionContext(_ctx, getState());
		enterRule(_localctx, 46, RULE_argCameraProjection);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(218);
			match(T__20);
			setState(219);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCameraIDContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgCameraIDContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCameraID; }
	}

	public final ArgCameraIDContext argCameraID() throws RecognitionException {
		ArgCameraIDContext _localctx = new ArgCameraIDContext(_ctx, getState());
		enterRule(_localctx, 48, RULE_argCameraID);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(221);
			match(T__21);
			setState(222);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCameraFrustumContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public ArgCameraFrustumContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCameraFrustum; }
	}

	public final ArgCameraFrustumContext argCameraFrustum() throws RecognitionException {
		ArgCameraFrustumContext _localctx = new ArgCameraFrustumContext(_ctx, getState());
		enterRule(_localctx, 50, RULE_argCameraFrustum);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(224);
			match(T__22);
			setState(225);
			match(LPAREN);
			setState(229);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==T__0 || _la==T__3 || ((((_la - 145)) & ~0x3f) == 0 && ((1L << (_la - 145)) & ((1L << (IDENT - 145)) | (1L << (SCIENTIFIC_NUMBER - 145)) | (1L << (LPAREN - 145)) | (1L << (PLUS - 145)) | (1L << (MINUS - 145)))) != 0)) {
				{
				{
				setState(226);
				expression(0);
				}
				}
				setState(231);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(232);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgTransformContext extends ParserRuleContext {
		public ArgTransformContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argTransform; }
	 
		public ArgTransformContext() { }
		public void copyFrom(ArgTransformContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class ArgTransformOneContext extends ArgTransformContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgTransformOneContext(ArgTransformContext ctx) { copyFrom(ctx); }
	}
	public static class ArgTransformTwoContext extends ArgTransformContext {
		public ExpressionContext exp1;
		public ExpressionContext exp2;
		public ExpressionContext exp3;
		public ExpressionContext exp4;
		public List<TerminalNode> LPAREN() { return getTokens(NomParser.LPAREN); }
		public TerminalNode LPAREN(int i) {
			return getToken(NomParser.LPAREN, i);
		}
		public List<TerminalNode> RPAREN() { return getTokens(NomParser.RPAREN); }
		public TerminalNode RPAREN(int i) {
			return getToken(NomParser.RPAREN, i);
		}
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public ArgTransformTwoContext(ArgTransformContext ctx) { copyFrom(ctx); }
	}

	public final ArgTransformContext argTransform() throws RecognitionException {
		ArgTransformContext _localctx = new ArgTransformContext(_ctx, getState());
		enterRule(_localctx, 52, RULE_argTransform);
		try {
			setState(258);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case T__23:
				_localctx = new ArgTransformTwoContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(234);
				match(T__23);
				setState(235);
				match(LPAREN);
				setState(236);
				((ArgTransformTwoContext)_localctx).exp1 = expression(0);
				setState(237);
				((ArgTransformTwoContext)_localctx).exp2 = expression(0);
				setState(238);
				((ArgTransformTwoContext)_localctx).exp3 = expression(0);
				setState(239);
				match(RPAREN);
				setState(240);
				match(LPAREN);
				setState(241);
				((ArgTransformTwoContext)_localctx).exp4 = expression(0);
				setState(242);
				match(RPAREN);
				}
				break;
			case T__24:
				_localctx = new ArgTransformOneContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(244);
				match(T__24);
				setState(245);
				match(LPAREN);
				setState(246);
				expression(0);
				setState(247);
				expression(0);
				setState(248);
				expression(0);
				setState(249);
				match(RPAREN);
				}
				break;
			case T__25:
				_localctx = new ArgTransformOneContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(251);
				match(T__25);
				setState(252);
				match(LPAREN);
				setState(253);
				expression(0);
				setState(254);
				expression(0);
				setState(255);
				expression(0);
				setState(256);
				match(RPAREN);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgRotateContext extends ParserRuleContext {
		public ExpressionContext exp1;
		public ExpressionContext exp2;
		public ExpressionContext exp3;
		public ExpressionContext exp4;
		public List<TerminalNode> LPAREN() { return getTokens(NomParser.LPAREN); }
		public TerminalNode LPAREN(int i) {
			return getToken(NomParser.LPAREN, i);
		}
		public List<TerminalNode> RPAREN() { return getTokens(NomParser.RPAREN); }
		public TerminalNode RPAREN(int i) {
			return getToken(NomParser.RPAREN, i);
		}
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public ArgRotateContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argRotate; }
	}

	public final ArgRotateContext argRotate() throws RecognitionException {
		ArgRotateContext _localctx = new ArgRotateContext(_ctx, getState());
		enterRule(_localctx, 54, RULE_argRotate);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(260);
			match(T__26);
			setState(261);
			match(LPAREN);
			setState(262);
			((ArgRotateContext)_localctx).exp1 = expression(0);
			setState(263);
			((ArgRotateContext)_localctx).exp2 = expression(0);
			setState(264);
			((ArgRotateContext)_localctx).exp3 = expression(0);
			setState(265);
			match(RPAREN);
			setState(266);
			match(LPAREN);
			setState(267);
			((ArgRotateContext)_localctx).exp4 = expression(0);
			setState(268);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgTranslateContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgTranslateContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argTranslate; }
	}

	public final ArgTranslateContext argTranslate() throws RecognitionException {
		ArgTranslateContext _localctx = new ArgTranslateContext(_ctx, getState());
		enterRule(_localctx, 56, RULE_argTranslate);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(270);
			match(T__27);
			setState(271);
			match(LPAREN);
			setState(272);
			expression(0);
			setState(273);
			expression(0);
			setState(274);
			expression(0);
			setState(275);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgColorContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgColorContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argColor; }
	}

	public final ArgColorContext argColor() throws RecognitionException {
		ArgColorContext _localctx = new ArgColorContext(_ctx, getState());
		enterRule(_localctx, 58, RULE_argColor);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(277);
			match(T__19);
			setState(278);
			match(LPAREN);
			setState(279);
			expression(0);
			setState(280);
			expression(0);
			setState(281);
			expression(0);
			setState(282);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgControlRotateContext extends ParserRuleContext {
		public Vector3Context vector3() {
			return getRuleContext(Vector3Context.class,0);
		}
		public ArgControlRotateContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argControlRotate; }
	}

	public final ArgControlRotateContext argControlRotate() throws RecognitionException {
		ArgControlRotateContext _localctx = new ArgControlRotateContext(_ctx, getState());
		enterRule(_localctx, 60, RULE_argControlRotate);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(284);
			match(T__23);
			setState(285);
			vector3();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgControlScaleContext extends ParserRuleContext {
		public Vector3Context vector3() {
			return getRuleContext(Vector3Context.class,0);
		}
		public ArgControlScaleContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argControlScale; }
	}

	public final ArgControlScaleContext argControlScale() throws RecognitionException {
		ArgControlScaleContext _localctx = new ArgControlScaleContext(_ctx, getState());
		enterRule(_localctx, 62, RULE_argControlScale);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(287);
			match(T__24);
			setState(288);
			vector3();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgPointContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgPointContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argPoint; }
	}

	public final ArgPointContext argPoint() throws RecognitionException {
		ArgPointContext _localctx = new ArgPointContext(_ctx, getState());
		enterRule(_localctx, 64, RULE_argPoint);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(290);
			match(T__28);
			setState(291);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgAzimuthContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgAzimuthContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argAzimuth; }
	}

	public final ArgAzimuthContext argAzimuth() throws RecognitionException {
		ArgAzimuthContext _localctx = new ArgAzimuthContext(_ctx, getState());
		enterRule(_localctx, 66, RULE_argAzimuth);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(293);
			match(T__29);
			setState(294);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgTwistContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgTwistContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argTwist; }
	}

	public final ArgTwistContext argTwist() throws RecognitionException {
		ArgTwistContext _localctx = new ArgTwistContext(_ctx, getState());
		enterRule(_localctx, 68, RULE_argTwist);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(296);
			match(T__30);
			setState(297);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgReverseContext extends ParserRuleContext {
		public ArgReverseContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argReverse; }
	}

	public final ArgReverseContext argReverse() throws RecognitionException {
		ArgReverseContext _localctx = new ArgReverseContext(_ctx, getState());
		enterRule(_localctx, 70, RULE_argReverse);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(299);
			match(T__31);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgMintorsionContext extends ParserRuleContext {
		public ArgMintorsionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argMintorsion; }
	}

	public final ArgMintorsionContext argMintorsion() throws RecognitionException {
		ArgMintorsionContext _localctx = new ArgMintorsionContext(_ctx, getState());
		enterRule(_localctx, 72, RULE_argMintorsion);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(301);
			match(T__32);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgOriginContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgOriginContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argOrigin; }
	}

	public final ArgOriginContext argOrigin() throws RecognitionException {
		ArgOriginContext _localctx = new ArgOriginContext(_ctx, getState());
		enterRule(_localctx, 74, RULE_argOrigin);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(303);
			match(T__33);
			setState(304);
			match(LPAREN);
			setState(305);
			expression(0);
			setState(306);
			expression(0);
			setState(307);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgSizeContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgSizeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argSize; }
	}

	public final ArgSizeContext argSize() throws RecognitionException {
		ArgSizeContext _localctx = new ArgSizeContext(_ctx, getState());
		enterRule(_localctx, 76, RULE_argSize);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(309);
			match(T__34);
			setState(310);
			match(LPAREN);
			setState(311);
			expression(0);
			setState(312);
			expression(0);
			setState(313);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgBackgroundContext extends ParserRuleContext {
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public ArgBackgroundContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argBackground; }
	}

	public final ArgBackgroundContext argBackground() throws RecognitionException {
		ArgBackgroundContext _localctx = new ArgBackgroundContext(_ctx, getState());
		enterRule(_localctx, 78, RULE_argBackground);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(315);
			match(T__35);
			setState(316);
			match(LPAREN);
			setState(317);
			expression(0);
			setState(318);
			expression(0);
			setState(319);
			expression(0);
			setState(320);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgFuncContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgFuncContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argFunc; }
	}

	public final ArgFuncContext argFunc() throws RecognitionException {
		ArgFuncContext _localctx = new ArgFuncContext(_ctx, getState());
		enterRule(_localctx, 80, RULE_argFunc);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(322);
			match(T__36);
			setState(323);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgFuncXContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgFuncXContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argFuncX; }
	}

	public final ArgFuncXContext argFuncX() throws RecognitionException {
		ArgFuncXContext _localctx = new ArgFuncXContext(_ctx, getState());
		enterRule(_localctx, 82, RULE_argFuncX);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(325);
			match(T__37);
			setState(326);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgFuncYContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgFuncYContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argFuncY; }
	}

	public final ArgFuncYContext argFuncY() throws RecognitionException {
		ArgFuncYContext _localctx = new ArgFuncYContext(_ctx, getState());
		enterRule(_localctx, 84, RULE_argFuncY);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(328);
			match(T__38);
			setState(329);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgFuncZContext extends ParserRuleContext {
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public ArgFuncZContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argFuncZ; }
	}

	public final ArgFuncZContext argFuncZ() throws RecognitionException {
		ArgFuncZContext _localctx = new ArgFuncZContext(_ctx, getState());
		enterRule(_localctx, 86, RULE_argFuncZ);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(331);
			match(T__39);
			setState(332);
			ident();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgBotCapContext extends ParserRuleContext {
		public ArgBotCapContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argBotCap; }
	}

	public final ArgBotCapContext argBotCap() throws RecognitionException {
		ArgBotCapContext _localctx = new ArgBotCapContext(_ctx, getState());
		enterRule(_localctx, 88, RULE_argBotCap);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(334);
			match(T__40);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgTopCapContext extends ParserRuleContext {
		public ArgTopCapContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argTopCap; }
	}

	public final ArgTopCapContext argTopCap() throws RecognitionException {
		ArgTopCapContext _localctx = new ArgTopCapContext(_ctx, getState());
		enterRule(_localctx, 90, RULE_argTopCap);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(336);
			match(T__41);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCutBeginContext extends ParserRuleContext {
		public ArgCutBeginContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCutBegin; }
	}

	public final ArgCutBeginContext argCutBegin() throws RecognitionException {
		ArgCutBeginContext _localctx = new ArgCutBeginContext(_ctx, getState());
		enterRule(_localctx, 92, RULE_argCutBegin);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(338);
			match(T__42);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgCutEndContext extends ParserRuleContext {
		public ArgCutEndContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argCutEnd; }
	}

	public final ArgCutEndContext argCutEnd() throws RecognitionException {
		ArgCutEndContext _localctx = new ArgCutEndContext(_ctx, getState());
		enterRule(_localctx, 94, RULE_argCutEnd);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(340);
			match(T__43);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ArgJoinContext extends ParserRuleContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ArgJoinContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_argJoin; }
	}

	public final ArgJoinContext argJoin() throws RecognitionException {
		ArgJoinContext _localctx = new ArgJoinContext(_ctx, getState());
		enterRule(_localctx, 96, RULE_argJoin);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(342);
			match(T__44);
			setState(343);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class CommandContext extends ParserRuleContext {
		public CommandContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_command; }
	 
		public CommandContext() { }
		public void copyFrom(CommandContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class CmdSurfaceContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgColorContext argColor() {
			return getRuleContext(ArgColorContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdSurfaceContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdInitColorContext extends CommandContext {
		public Token open;
		public Token end;
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public CmdInitColorContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdInstanceContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public IdentContext entity;
		public Token end;
		public List<IdentContext> ident() {
			return getRuleContexts(IdentContext.class);
		}
		public IdentContext ident(int i) {
			return getRuleContext(IdentContext.class,i);
		}
		public List<ArgSurfaceContext> argSurface() {
			return getRuleContexts(ArgSurfaceContext.class);
		}
		public ArgSurfaceContext argSurface(int i) {
			return getRuleContext(ArgSurfaceContext.class,i);
		}
		public List<ArgTransformContext> argTransform() {
			return getRuleContexts(ArgTransformContext.class);
		}
		public ArgTransformContext argTransform(int i) {
			return getRuleContext(ArgTransformContext.class,i);
		}
		public List<ArgHiddenContext> argHidden() {
			return getRuleContexts(ArgHiddenContext.class);
		}
		public ArgHiddenContext argHidden(int i) {
			return getRuleContext(ArgHiddenContext.class,i);
		}
		public List<ArgBackfaceContext> argBackface() {
			return getRuleContexts(ArgBackfaceContext.class);
		}
		public ArgBackfaceContext argBackface(int i) {
			return getRuleContext(ArgBackfaceContext.class,i);
		}
		public CmdInstanceContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdWindowContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgOriginContext argOrigin() {
			return getRuleContext(ArgOriginContext.class,0);
		}
		public ArgSizeContext argSize() {
			return getRuleContext(ArgSizeContext.class,0);
		}
		public ArgBackgroundContext argBackground() {
			return getRuleContext(ArgBackgroundContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdWindowContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdDeleteContext extends CommandContext {
		public Token open;
		public Token end;
		public List<DeleteFaceContext> deleteFace() {
			return getRuleContexts(DeleteFaceContext.class);
		}
		public DeleteFaceContext deleteFace(int i) {
			return getRuleContext(DeleteFaceContext.class,i);
		}
		public CmdDeleteContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdLightContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgLightTypeContext argLightType() {
			return getRuleContext(ArgLightTypeContext.class,0);
		}
		public ArgLightColorContext argLightColor() {
			return getRuleContext(ArgLightColorContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdLightContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdNamedArgsContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgPointContext argPoint() {
			return getRuleContext(ArgPointContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ArgControlScaleContext> argControlScale() {
			return getRuleContexts(ArgControlScaleContext.class);
		}
		public ArgControlScaleContext argControlScale(int i) {
			return getRuleContext(ArgControlScaleContext.class,i);
		}
		public List<ArgControlRotateContext> argControlRotate() {
			return getRuleContexts(ArgControlRotateContext.class);
		}
		public ArgControlRotateContext argControlRotate(int i) {
			return getRuleContext(ArgControlRotateContext.class,i);
		}
		public List<ArgCrossContext> argCross() {
			return getRuleContexts(ArgCrossContext.class);
		}
		public ArgCrossContext argCross(int i) {
			return getRuleContext(ArgCrossContext.class,i);
		}
		public List<ArgReverseContext> argReverse() {
			return getRuleContexts(ArgReverseContext.class);
		}
		public ArgReverseContext argReverse(int i) {
			return getRuleContext(ArgReverseContext.class,i);
		}
		public CmdNamedArgsContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdCameraContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgCameraProjectionContext argCameraProjection() {
			return getRuleContext(ArgCameraProjectionContext.class,0);
		}
		public ArgCameraFrustumContext argCameraFrustum() {
			return getRuleContext(ArgCameraFrustumContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdCameraContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdViewportContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgCameraIDContext argCameraID() {
			return getRuleContext(ArgCameraIDContext.class,0);
		}
		public ArgOriginContext argOrigin() {
			return getRuleContext(ArgOriginContext.class,0);
		}
		public ArgSizeContext argSize() {
			return getRuleContext(ArgSizeContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdViewportContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdExprListOneContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ArgBotCapContext> argBotCap() {
			return getRuleContexts(ArgBotCapContext.class);
		}
		public ArgBotCapContext argBotCap(int i) {
			return getRuleContext(ArgBotCapContext.class,i);
		}
		public List<ArgTopCapContext> argTopCap() {
			return getRuleContexts(ArgTopCapContext.class);
		}
		public ArgTopCapContext argTopCap(int i) {
			return getRuleContext(ArgTopCapContext.class,i);
		}
		public CmdExprListOneContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdIdListOneContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public IdListContext idList() {
			return getRuleContext(IdListContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ArgClosedContext> argClosed() {
			return getRuleContexts(ArgClosedContext.class);
		}
		public ArgClosedContext argClosed(int i) {
			return getRuleContext(ArgClosedContext.class,i);
		}
		public List<ArgSurfaceContext> argSurface() {
			return getRuleContexts(ArgSurfaceContext.class);
		}
		public ArgSurfaceContext argSurface(int i) {
			return getRuleContext(ArgSurfaceContext.class,i);
		}
		public List<ArgBackfaceContext> argBackface() {
			return getRuleContexts(ArgBackfaceContext.class);
		}
		public ArgBackfaceContext argBackface(int i) {
			return getRuleContext(ArgBackfaceContext.class,i);
		}
		public List<ArgSegsContext> argSegs() {
			return getRuleContexts(ArgSegsContext.class);
		}
		public ArgSegsContext argSegs(int i) {
			return getRuleContext(ArgSegsContext.class,i);
		}
		public List<ArgOrderContext> argOrder() {
			return getRuleContexts(ArgOrderContext.class);
		}
		public ArgOrderContext argOrder(int i) {
			return getRuleContext(ArgOrderContext.class,i);
		}
		public CmdIdListOneContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdSharpContext extends CommandContext {
		public Token open;
		public Token end;
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public List<IdListContext> idList() {
			return getRuleContexts(IdListContext.class);
		}
		public IdListContext idList(int i) {
			return getRuleContext(IdListContext.class,i);
		}
		public CmdSharpContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdGeneralContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgFuncContext argFunc() {
			return getRuleContext(ArgFuncContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdGeneralContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdArgSurfaceContext extends CommandContext {
		public Token open;
		public Token end;
		public ArgSurfaceContext argSurface() {
			return getRuleContext(ArgSurfaceContext.class,0);
		}
		public CmdArgSurfaceContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdSweepContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public IdentContext crossId;
		public IdentContext pathId;
		public Token end;
		public List<IdentContext> ident() {
			return getRuleContexts(IdentContext.class);
		}
		public IdentContext ident(int i) {
			return getRuleContext(IdentContext.class,i);
		}
		public List<ArgBeginCapContext> argBeginCap() {
			return getRuleContexts(ArgBeginCapContext.class);
		}
		public ArgBeginCapContext argBeginCap(int i) {
			return getRuleContext(ArgBeginCapContext.class,i);
		}
		public List<ArgEndCapContext> argEndCap() {
			return getRuleContexts(ArgEndCapContext.class);
		}
		public ArgEndCapContext argEndCap(int i) {
			return getRuleContext(ArgEndCapContext.class,i);
		}
		public List<ArgReverseContext> argReverse() {
			return getRuleContexts(ArgReverseContext.class);
		}
		public ArgReverseContext argReverse(int i) {
			return getRuleContext(ArgReverseContext.class,i);
		}
		public List<ArgAzimuthContext> argAzimuth() {
			return getRuleContexts(ArgAzimuthContext.class);
		}
		public ArgAzimuthContext argAzimuth(int i) {
			return getRuleContext(ArgAzimuthContext.class,i);
		}
		public List<ArgTwistContext> argTwist() {
			return getRuleContexts(ArgTwistContext.class);
		}
		public ArgTwistContext argTwist(int i) {
			return getRuleContext(ArgTwistContext.class,i);
		}
		public List<ArgMintorsionContext> argMintorsion() {
			return getRuleContexts(ArgMintorsionContext.class);
		}
		public ArgMintorsionContext argMintorsion(int i) {
			return getRuleContext(ArgMintorsionContext.class,i);
		}
		public List<ArgCutBeginContext> argCutBegin() {
			return getRuleContexts(ArgCutBeginContext.class);
		}
		public ArgCutBeginContext argCutBegin(int i) {
			return getRuleContext(ArgCutBeginContext.class,i);
		}
		public List<ArgCutEndContext> argCutEnd() {
			return getRuleContexts(ArgCutEndContext.class);
		}
		public ArgCutEndContext argCutEnd(int i) {
			return getRuleContext(ArgCutEndContext.class,i);
		}
		public List<ArgJoinContext> argJoin() {
			return getRuleContexts(ArgJoinContext.class);
		}
		public ArgJoinContext argJoin(int i) {
			return getRuleContext(ArgJoinContext.class,i);
		}
		public CmdSweepContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdSubdivisionContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ArgSdFlagContext> argSdFlag() {
			return getRuleContexts(ArgSdFlagContext.class);
		}
		public ArgSdFlagContext argSdFlag(int i) {
			return getRuleContext(ArgSdFlagContext.class,i);
		}
		public List<ArgSdLevelContext> argSdLevel() {
			return getRuleContexts(ArgSdLevelContext.class);
		}
		public ArgSdLevelContext argSdLevel(int i) {
			return getRuleContext(ArgSdLevelContext.class,i);
		}
		public List<CommandContext> command() {
			return getRuleContexts(CommandContext.class);
		}
		public CommandContext command(int i) {
			return getRuleContext(CommandContext.class,i);
		}
		public CmdSubdivisionContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdBankContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ListContext list() {
			return getRuleContext(ListContext.class,0);
		}
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<SetContext> set() {
			return getRuleContexts(SetContext.class);
		}
		public SetContext set(int i) {
			return getRuleContext(SetContext.class,i);
		}
		public CmdBankContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdOffsetContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ArgOffsetFlagContext> argOffsetFlag() {
			return getRuleContexts(ArgOffsetFlagContext.class);
		}
		public ArgOffsetFlagContext argOffsetFlag(int i) {
			return getRuleContext(ArgOffsetFlagContext.class,i);
		}
		public List<ArgHeightContext> argHeight() {
			return getRuleContexts(ArgHeightContext.class);
		}
		public ArgHeightContext argHeight(int i) {
			return getRuleContext(ArgHeightContext.class,i);
		}
		public List<ArgWidthContext> argWidth() {
			return getRuleContexts(ArgWidthContext.class);
		}
		public ArgWidthContext argWidth(int i) {
			return getRuleContext(ArgWidthContext.class,i);
		}
		public List<CommandContext> command() {
			return getRuleContexts(CommandContext.class);
		}
		public CommandContext command(int i) {
			return getRuleContext(CommandContext.class,i);
		}
		public CmdOffsetContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdGeneralParametricContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public ArgFuncXContext argFuncX() {
			return getRuleContext(ArgFuncXContext.class,0);
		}
		public ArgFuncYContext argFuncY() {
			return getRuleContext(ArgFuncYContext.class,0);
		}
		public ArgFuncZContext argFuncZ() {
			return getRuleContext(ArgFuncZContext.class,0);
		}
		public TerminalNode LPAREN() { return getToken(NomParser.LPAREN, 0); }
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public TerminalNode RPAREN() { return getToken(NomParser.RPAREN, 0); }
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdGeneralParametricContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdSubCmdsContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<CommandContext> command() {
			return getRuleContexts(CommandContext.class);
		}
		public CommandContext command(int i) {
			return getRuleContext(CommandContext.class,i);
		}
		public CmdSubCmdsContext(CommandContext ctx) { copyFrom(ctx); }
	}
	public static class CmdIncludeContext extends CommandContext {
		public Token open;
		public IdentContext name;
		public Token end;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public CmdIncludeContext(CommandContext ctx) { copyFrom(ctx); }
	}

	public final CommandContext command() throws RecognitionException {
		CommandContext _localctx = new CommandContext(_ctx, getState());
		enterRule(_localctx, 98, RULE_command);
		int _la;
		try {
			setState(848);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case T__28:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(345);
				((CmdExprListOneContext)_localctx).open = match(T__28);
				setState(346);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(347);
				match(LPAREN);
				setState(348);
				expression(0);
				setState(349);
				expression(0);
				setState(350);
				expression(0);
				setState(351);
				match(RPAREN);
				setState(352);
				((CmdExprListOneContext)_localctx).end = match(T__45);
				}
				break;
			case T__46:
				_localctx = new CmdIdListOneContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(354);
				((CmdIdListOneContext)_localctx).open = match(T__46);
				setState(355);
				((CmdIdListOneContext)_localctx).name = ident();
				setState(356);
				idList();
				setState(360);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__4) {
					{
					{
					setState(357);
					argClosed();
					}
					}
					setState(362);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(363);
				((CmdIdListOneContext)_localctx).end = match(T__47);
				}
				break;
			case T__48:
				_localctx = new CmdSweepContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(365);
				((CmdSweepContext)_localctx).open = match(T__48);
				setState(366);
				((CmdSweepContext)_localctx).name = ident();
				setState(367);
				match(T__49);
				setState(368);
				((CmdSweepContext)_localctx).crossId = ident();
				setState(374);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__11) | (1L << T__12) | (1L << T__31))) != 0)) {
					{
					setState(372);
					_errHandler.sync(this);
					switch (_input.LA(1)) {
					case T__11:
						{
						setState(369);
						argBeginCap();
						}
						break;
					case T__12:
						{
						setState(370);
						argEndCap();
						}
						break;
					case T__31:
						{
						setState(371);
						argReverse();
						}
						break;
					default:
						throw new NoViableAltException(this);
					}
					}
					setState(376);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(377);
				match(T__50);
				setState(378);
				match(T__51);
				setState(379);
				((CmdSweepContext)_localctx).pathId = ident();
				setState(388);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__29) | (1L << T__30) | (1L << T__32) | (1L << T__42) | (1L << T__43) | (1L << T__44))) != 0)) {
					{
					setState(386);
					_errHandler.sync(this);
					switch (_input.LA(1)) {
					case T__29:
						{
						setState(380);
						argAzimuth();
						}
						break;
					case T__30:
						{
						setState(381);
						argTwist();
						}
						break;
					case T__32:
						{
						setState(382);
						argMintorsion();
						}
						break;
					case T__42:
						{
						setState(383);
						argCutBegin();
						}
						break;
					case T__43:
						{
						setState(384);
						argCutEnd();
						}
						break;
					case T__44:
						{
						setState(385);
						argJoin();
						}
						break;
					default:
						throw new NoViableAltException(this);
					}
					}
					setState(390);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(391);
				match(T__52);
				setState(392);
				((CmdSweepContext)_localctx).end = match(T__53);
				}
				break;
			case T__54:
				_localctx = new CmdIdListOneContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(394);
				((CmdIdListOneContext)_localctx).open = match(T__54);
				setState(395);
				((CmdIdListOneContext)_localctx).name = ident();
				setState(396);
				idList();
				setState(400);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__13) {
					{
					{
					setState(397);
					argSurface();
					}
					}
					setState(402);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(406);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__14) {
					{
					{
					setState(403);
					argBackface();
					}
					}
					setState(408);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(409);
				((CmdIdListOneContext)_localctx).end = match(T__55);
				}
				break;
			case T__56:
				_localctx = new CmdNamedArgsContext(_localctx);
				enterOuterAlt(_localctx, 5);
				{
				setState(411);
				((CmdNamedArgsContext)_localctx).open = match(T__56);
				setState(412);
				((CmdNamedArgsContext)_localctx).name = ident();
				setState(413);
				argPoint();
				setState(420);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__15) | (1L << T__23) | (1L << T__24) | (1L << T__31))) != 0)) {
					{
					setState(418);
					_errHandler.sync(this);
					switch (_input.LA(1)) {
					case T__24:
						{
						setState(414);
						argControlScale();
						}
						break;
					case T__23:
						{
						setState(415);
						argControlRotate();
						}
						break;
					case T__15:
						{
						setState(416);
						argCross();
						}
						break;
					case T__31:
						{
						setState(417);
						argReverse();
						}
						break;
					default:
						throw new NoViableAltException(this);
					}
					}
					setState(422);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(423);
				((CmdNamedArgsContext)_localctx).end = match(T__57);
				}
				break;
			case T__58:
				_localctx = new CmdIdListOneContext(_localctx);
				enterOuterAlt(_localctx, 6);
				{
				setState(425);
				((CmdIdListOneContext)_localctx).open = match(T__58);
				setState(426);
				((CmdIdListOneContext)_localctx).name = ident();
				setState(427);
				idList();
				setState(428);
				((CmdIdListOneContext)_localctx).end = match(T__59);
				}
				break;
			case T__60:
				_localctx = new CmdSubCmdsContext(_localctx);
				enterOuterAlt(_localctx, 7);
				{
				setState(430);
				((CmdSubCmdsContext)_localctx).open = match(T__60);
				setState(431);
				((CmdSubCmdsContext)_localctx).name = ident();
				setState(435);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (((((_la - 14)) & ~0x3f) == 0 && ((1L << (_la - 14)) & ((1L << (T__13 - 14)) | (1L << (T__14 - 14)) | (1L << (T__28 - 14)) | (1L << (T__46 - 14)) | (1L << (T__48 - 14)) | (1L << (T__54 - 14)) | (1L << (T__56 - 14)) | (1L << (T__58 - 14)) | (1L << (T__60 - 14)) | (1L << (T__62 - 14)) | (1L << (T__64 - 14)) | (1L << (T__66 - 14)) | (1L << (T__68 - 14)) | (1L << (T__70 - 14)) | (1L << (T__72 - 14)) | (1L << (T__74 - 14)) | (1L << (T__76 - 14)))) != 0) || ((((_la - 79)) & ~0x3f) == 0 && ((1L << (_la - 79)) & ((1L << (T__78 - 79)) | (1L << (T__80 - 79)) | (1L << (T__82 - 79)) | (1L << (T__84 - 79)) | (1L << (T__86 - 79)) | (1L << (T__88 - 79)) | (1L << (T__90 - 79)) | (1L << (T__92 - 79)) | (1L << (T__94 - 79)) | (1L << (T__96 - 79)) | (1L << (T__98 - 79)) | (1L << (T__100 - 79)) | (1L << (T__103 - 79)) | (1L << (T__105 - 79)) | (1L << (T__108 - 79)) | (1L << (T__110 - 79)) | (1L << (T__112 - 79)) | (1L << (T__114 - 79)) | (1L << (T__116 - 79)) | (1L << (T__118 - 79)) | (1L << (T__120 - 79)) | (1L << (T__122 - 79)) | (1L << (T__124 - 79)) | (1L << (T__126 - 79)) | (1L << (T__128 - 79)) | (1L << (T__130 - 79)) | (1L << (T__132 - 79)) | (1L << (T__134 - 79)) | (1L << (T__136 - 79)) | (1L << (T__138 - 79)) | (1L << (T__140 - 79)))) != 0)) {
					{
					{
					setState(432);
					command();
					}
					}
					setState(437);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(438);
				((CmdSubCmdsContext)_localctx).end = match(T__61);
				}
				break;
			case T__62:
				_localctx = new CmdSubCmdsContext(_localctx);
				enterOuterAlt(_localctx, 8);
				{
				setState(440);
				((CmdSubCmdsContext)_localctx).open = match(T__62);
				setState(441);
				((CmdSubCmdsContext)_localctx).name = ident();
				setState(445);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (((((_la - 14)) & ~0x3f) == 0 && ((1L << (_la - 14)) & ((1L << (T__13 - 14)) | (1L << (T__14 - 14)) | (1L << (T__28 - 14)) | (1L << (T__46 - 14)) | (1L << (T__48 - 14)) | (1L << (T__54 - 14)) | (1L << (T__56 - 14)) | (1L << (T__58 - 14)) | (1L << (T__60 - 14)) | (1L << (T__62 - 14)) | (1L << (T__64 - 14)) | (1L << (T__66 - 14)) | (1L << (T__68 - 14)) | (1L << (T__70 - 14)) | (1L << (T__72 - 14)) | (1L << (T__74 - 14)) | (1L << (T__76 - 14)))) != 0) || ((((_la - 79)) & ~0x3f) == 0 && ((1L << (_la - 79)) & ((1L << (T__78 - 79)) | (1L << (T__80 - 79)) | (1L << (T__82 - 79)) | (1L << (T__84 - 79)) | (1L << (T__86 - 79)) | (1L << (T__88 - 79)) | (1L << (T__90 - 79)) | (1L << (T__92 - 79)) | (1L << (T__94 - 79)) | (1L << (T__96 - 79)) | (1L << (T__98 - 79)) | (1L << (T__100 - 79)) | (1L << (T__103 - 79)) | (1L << (T__105 - 79)) | (1L << (T__108 - 79)) | (1L << (T__110 - 79)) | (1L << (T__112 - 79)) | (1L << (T__114 - 79)) | (1L << (T__116 - 79)) | (1L << (T__118 - 79)) | (1L << (T__120 - 79)) | (1L << (T__122 - 79)) | (1L << (T__124 - 79)) | (1L << (T__126 - 79)) | (1L << (T__128 - 79)) | (1L << (T__130 - 79)) | (1L << (T__132 - 79)) | (1L << (T__134 - 79)) | (1L << (T__136 - 79)) | (1L << (T__138 - 79)) | (1L << (T__140 - 79)))) != 0)) {
					{
					{
					setState(442);
					command();
					}
					}
					setState(447);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(448);
				((CmdSubCmdsContext)_localctx).end = match(T__63);
				}
				break;
			case T__64:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 9);
				{
				setState(450);
				((CmdExprListOneContext)_localctx).open = match(T__64);
				setState(451);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(452);
				match(LPAREN);
				setState(453);
				expression(0);
				setState(454);
				expression(0);
				setState(455);
				match(RPAREN);
				setState(456);
				((CmdExprListOneContext)_localctx).end = match(T__65);
				}
				break;
			case T__66:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 10);
				{
				setState(458);
				((CmdExprListOneContext)_localctx).open = match(T__66);
				setState(459);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(460);
				match(LPAREN);
				setState(461);
				expression(0);
				setState(462);
				expression(0);
				setState(463);
				expression(0);
				setState(464);
				match(RPAREN);
				setState(465);
				((CmdExprListOneContext)_localctx).end = match(T__67);
				}
				break;
			case T__68:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 11);
				{
				setState(467);
				((CmdExprListOneContext)_localctx).open = match(T__68);
				setState(468);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(469);
				match(LPAREN);
				setState(470);
				expression(0);
				setState(471);
				expression(0);
				setState(472);
				expression(0);
				setState(473);
				expression(0);
				setState(474);
				expression(0);
				setState(475);
				expression(0);
				setState(476);
				match(RPAREN);
				setState(477);
				((CmdExprListOneContext)_localctx).end = match(T__69);
				}
				break;
			case T__70:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 12);
				{
				setState(479);
				((CmdExprListOneContext)_localctx).open = match(T__70);
				setState(480);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(481);
				match(LPAREN);
				setState(482);
				expression(0);
				setState(483);
				expression(0);
				setState(484);
				expression(0);
				setState(485);
				expression(0);
				setState(486);
				expression(0);
				setState(487);
				expression(0);
				setState(488);
				expression(0);
				setState(489);
				match(RPAREN);
				setState(490);
				((CmdExprListOneContext)_localctx).end = match(T__71);
				}
				break;
			case T__72:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 13);
				{
				setState(492);
				((CmdExprListOneContext)_localctx).open = match(T__72);
				setState(493);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(494);
				match(LPAREN);
				setState(495);
				expression(0);
				setState(496);
				expression(0);
				setState(497);
				expression(0);
				setState(498);
				expression(0);
				setState(503);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__40 || _la==T__41) {
					{
					setState(501);
					_errHandler.sync(this);
					switch (_input.LA(1)) {
					case T__40:
						{
						setState(499);
						argBotCap();
						}
						break;
					case T__41:
						{
						setState(500);
						argTopCap();
						}
						break;
					default:
						throw new NoViableAltException(this);
					}
					}
					setState(505);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(506);
				match(RPAREN);
				setState(507);
				((CmdExprListOneContext)_localctx).end = match(T__73);
				}
				break;
			case T__74:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 14);
				{
				setState(509);
				((CmdExprListOneContext)_localctx).open = match(T__74);
				setState(510);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(511);
				match(LPAREN);
				setState(512);
				expression(0);
				setState(513);
				expression(0);
				setState(514);
				expression(0);
				setState(515);
				expression(0);
				setState(516);
				expression(0);
				setState(517);
				expression(0);
				setState(518);
				match(RPAREN);
				setState(519);
				((CmdExprListOneContext)_localctx).end = match(T__75);
				}
				break;
			case T__76:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 15);
				{
				setState(521);
				((CmdExprListOneContext)_localctx).open = match(T__76);
				setState(522);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(523);
				match(LPAREN);
				setState(524);
				expression(0);
				setState(525);
				expression(0);
				setState(526);
				expression(0);
				setState(527);
				expression(0);
				setState(528);
				expression(0);
				setState(529);
				expression(0);
				setState(530);
				expression(0);
				setState(531);
				match(RPAREN);
				setState(532);
				((CmdExprListOneContext)_localctx).end = match(T__77);
				}
				break;
			case T__78:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 16);
				{
				setState(534);
				((CmdExprListOneContext)_localctx).open = match(T__78);
				setState(535);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(536);
				match(LPAREN);
				setState(537);
				expression(0);
				setState(538);
				expression(0);
				setState(539);
				expression(0);
				setState(540);
				expression(0);
				setState(541);
				match(RPAREN);
				setState(542);
				((CmdExprListOneContext)_localctx).end = match(T__79);
				}
				break;
			case T__80:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 17);
				{
				setState(544);
				((CmdExprListOneContext)_localctx).open = match(T__80);
				setState(545);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(546);
				match(LPAREN);
				setState(547);
				expression(0);
				setState(548);
				expression(0);
				setState(549);
				expression(0);
				setState(550);
				expression(0);
				setState(551);
				match(RPAREN);
				setState(552);
				((CmdExprListOneContext)_localctx).end = match(T__81);
				}
				break;
			case T__82:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 18);
				{
				setState(554);
				((CmdExprListOneContext)_localctx).open = match(T__82);
				setState(555);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(556);
				match(LPAREN);
				setState(557);
				expression(0);
				setState(558);
				expression(0);
				setState(559);
				expression(0);
				setState(560);
				expression(0);
				setState(561);
				match(RPAREN);
				setState(562);
				((CmdExprListOneContext)_localctx).end = match(T__83);
				}
				break;
			case T__84:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 19);
				{
				setState(564);
				((CmdExprListOneContext)_localctx).open = match(T__84);
				setState(565);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(566);
				match(LPAREN);
				setState(567);
				expression(0);
				setState(568);
				expression(0);
				setState(569);
				expression(0);
				setState(570);
				expression(0);
				setState(571);
				match(RPAREN);
				setState(572);
				((CmdExprListOneContext)_localctx).end = match(T__85);
				}
				break;
			case T__86:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 20);
				{
				setState(574);
				((CmdExprListOneContext)_localctx).open = match(T__86);
				setState(575);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(576);
				match(LPAREN);
				setState(577);
				expression(0);
				setState(578);
				expression(0);
				setState(579);
				expression(0);
				setState(580);
				expression(0);
				setState(581);
				expression(0);
				setState(582);
				expression(0);
				setState(583);
				expression(0);
				setState(584);
				match(RPAREN);
				setState(585);
				((CmdExprListOneContext)_localctx).end = match(T__87);
				}
				break;
			case T__88:
				_localctx = new CmdExprListOneContext(_localctx);
				enterOuterAlt(_localctx, 21);
				{
				setState(587);
				((CmdExprListOneContext)_localctx).open = match(T__88);
				setState(588);
				((CmdExprListOneContext)_localctx).name = ident();
				setState(589);
				match(LPAREN);
				setState(590);
				expression(0);
				setState(591);
				expression(0);
				setState(592);
				expression(0);
				setState(593);
				expression(0);
				setState(594);
				expression(0);
				setState(595);
				expression(0);
				setState(596);
				expression(0);
				setState(597);
				match(RPAREN);
				setState(598);
				((CmdExprListOneContext)_localctx).end = match(T__89);
				}
				break;
			case T__90:
				_localctx = new CmdGeneralContext(_localctx);
				enterOuterAlt(_localctx, 22);
				{
				setState(600);
				((CmdGeneralContext)_localctx).open = match(T__90);
				setState(601);
				((CmdGeneralContext)_localctx).name = ident();
				setState(602);
				argFunc();
				setState(603);
				match(LPAREN);
				setState(604);
				expression(0);
				setState(605);
				expression(0);
				setState(606);
				expression(0);
				setState(607);
				expression(0);
				setState(608);
				expression(0);
				setState(609);
				expression(0);
				setState(610);
				match(RPAREN);
				setState(611);
				((CmdGeneralContext)_localctx).end = match(T__91);
				}
				break;
			case T__92:
				_localctx = new CmdGeneralParametricContext(_localctx);
				enterOuterAlt(_localctx, 23);
				{
				setState(613);
				((CmdGeneralParametricContext)_localctx).open = match(T__92);
				setState(614);
				((CmdGeneralParametricContext)_localctx).name = ident();
				setState(615);
				argFuncX();
				setState(616);
				argFuncY();
				setState(617);
				argFuncZ();
				setState(618);
				match(LPAREN);
				setState(619);
				expression(0);
				setState(620);
				expression(0);
				setState(621);
				expression(0);
				setState(622);
				expression(0);
				setState(623);
				expression(0);
				setState(624);
				expression(0);
				setState(625);
				match(RPAREN);
				setState(626);
				((CmdGeneralParametricContext)_localctx).end = match(T__93);
				}
				break;
			case T__94:
				_localctx = new CmdGeneralContext(_localctx);
				enterOuterAlt(_localctx, 24);
				{
				setState(628);
				((CmdGeneralContext)_localctx).open = match(T__94);
				setState(629);
				((CmdGeneralContext)_localctx).name = ident();
				setState(630);
				argFunc();
				setState(631);
				match(LPAREN);
				setState(632);
				expression(0);
				setState(633);
				expression(0);
				setState(634);
				expression(0);
				setState(635);
				expression(0);
				setState(636);
				expression(0);
				setState(637);
				expression(0);
				setState(638);
				expression(0);
				setState(639);
				expression(0);
				setState(640);
				expression(0);
				setState(641);
				match(RPAREN);
				setState(642);
				((CmdGeneralContext)_localctx).end = match(T__95);
				}
				break;
			case T__96:
				_localctx = new CmdIdListOneContext(_localctx);
				enterOuterAlt(_localctx, 25);
				{
				setState(644);
				((CmdIdListOneContext)_localctx).open = match(T__96);
				setState(645);
				((CmdIdListOneContext)_localctx).name = ident();
				setState(646);
				idList();
				setState(650);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__16) {
					{
					{
					setState(647);
					argSegs();
					}
					}
					setState(652);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(653);
				((CmdIdListOneContext)_localctx).end = match(T__97);
				}
				break;
			case T__98:
				_localctx = new CmdIdListOneContext(_localctx);
				enterOuterAlt(_localctx, 26);
				{
				setState(655);
				((CmdIdListOneContext)_localctx).open = match(T__98);
				setState(656);
				((CmdIdListOneContext)_localctx).name = ident();
				setState(660);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__17) {
					{
					{
					setState(657);
					argOrder();
					}
					}
					setState(662);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(663);
				idList();
				setState(667);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__16) {
					{
					{
					setState(664);
					argSegs();
					}
					}
					setState(669);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(670);
				((CmdIdListOneContext)_localctx).end = match(T__99);
				}
				break;
			case T__100:
				_localctx = new CmdInstanceContext(_localctx);
				enterOuterAlt(_localctx, 27);
				{
				setState(672);
				((CmdInstanceContext)_localctx).open = match(T__100);
				setState(673);
				((CmdInstanceContext)_localctx).name = ident();
				setState(674);
				((CmdInstanceContext)_localctx).entity = ident();
				setState(681);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__10) | (1L << T__13) | (1L << T__14) | (1L << T__23) | (1L << T__24) | (1L << T__25))) != 0)) {
					{
					setState(679);
					_errHandler.sync(this);
					switch (_input.LA(1)) {
					case T__13:
						{
						setState(675);
						argSurface();
						}
						break;
					case T__23:
					case T__24:
					case T__25:
						{
						setState(676);
						argTransform();
						}
						break;
					case T__10:
						{
						setState(677);
						argHidden();
						}
						break;
					case T__14:
						{
						setState(678);
						argBackface();
						}
						break;
					default:
						throw new NoViableAltException(this);
					}
					}
					setState(683);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(684);
				((CmdInstanceContext)_localctx).end = match(T__101);
				}
				break;
			case T__13:
				_localctx = new CmdSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 28);
				{
				setState(686);
				((CmdSurfaceContext)_localctx).open = match(T__13);
				setState(687);
				((CmdSurfaceContext)_localctx).name = ident();
				setState(688);
				argColor();
				setState(689);
				((CmdSurfaceContext)_localctx).end = match(T__102);
				}
				break;
			case T__103:
				_localctx = new CmdInitColorContext(_localctx);
				enterOuterAlt(_localctx, 29);
				{
				setState(691);
				((CmdInitColorContext)_localctx).open = match(T__103);
				setState(692);
				match(LPAREN);
				setState(693);
				expression(0);
				setState(694);
				expression(0);
				setState(695);
				expression(0);
				setState(696);
				match(RPAREN);
				setState(697);
				((CmdInitColorContext)_localctx).end = match(T__104);
				}
				break;
			case T__105:
				_localctx = new CmdInitColorContext(_localctx);
				enterOuterAlt(_localctx, 30);
				{
				setState(699);
				((CmdInitColorContext)_localctx).open = match(T__105);
				setState(700);
				match(LPAREN);
				setState(701);
				expression(0);
				setState(702);
				expression(0);
				setState(703);
				expression(0);
				setState(704);
				match(RPAREN);
				setState(705);
				((CmdInitColorContext)_localctx).end = match(T__106);
				}
				break;
			case T__14:
				_localctx = new CmdSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 31);
				{
				setState(707);
				((CmdSurfaceContext)_localctx).open = match(T__14);
				setState(708);
				((CmdSurfaceContext)_localctx).name = ident();
				setState(709);
				argColor();
				setState(710);
				((CmdSurfaceContext)_localctx).end = match(T__107);
				}
				break;
			case T__108:
				_localctx = new CmdWindowContext(_localctx);
				enterOuterAlt(_localctx, 32);
				{
				setState(712);
				((CmdWindowContext)_localctx).open = match(T__108);
				setState(713);
				((CmdWindowContext)_localctx).name = ident();
				setState(714);
				argOrigin();
				setState(715);
				argSize();
				setState(716);
				argBackground();
				setState(717);
				((CmdWindowContext)_localctx).end = match(T__109);
				}
				break;
			case T__110:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 33);
				{
				setState(719);
				((CmdArgSurfaceContext)_localctx).open = match(T__110);
				setState(720);
				argSurface();
				setState(721);
				((CmdArgSurfaceContext)_localctx).end = match(T__111);
				}
				break;
			case T__112:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 34);
				{
				setState(723);
				((CmdArgSurfaceContext)_localctx).open = match(T__112);
				setState(724);
				argSurface();
				setState(725);
				((CmdArgSurfaceContext)_localctx).end = match(T__113);
				}
				break;
			case T__114:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 35);
				{
				setState(727);
				((CmdArgSurfaceContext)_localctx).open = match(T__114);
				setState(728);
				argSurface();
				setState(729);
				((CmdArgSurfaceContext)_localctx).end = match(T__115);
				}
				break;
			case T__116:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 36);
				{
				setState(731);
				((CmdArgSurfaceContext)_localctx).open = match(T__116);
				setState(732);
				argSurface();
				setState(733);
				((CmdArgSurfaceContext)_localctx).end = match(T__117);
				}
				break;
			case T__118:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 37);
				{
				setState(735);
				((CmdArgSurfaceContext)_localctx).open = match(T__118);
				setState(736);
				argSurface();
				setState(737);
				((CmdArgSurfaceContext)_localctx).end = match(T__119);
				}
				break;
			case T__120:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 38);
				{
				setState(739);
				((CmdArgSurfaceContext)_localctx).open = match(T__120);
				setState(740);
				argSurface();
				setState(741);
				((CmdArgSurfaceContext)_localctx).end = match(T__121);
				}
				break;
			case T__122:
				_localctx = new CmdArgSurfaceContext(_localctx);
				enterOuterAlt(_localctx, 39);
				{
				setState(743);
				((CmdArgSurfaceContext)_localctx).open = match(T__122);
				setState(744);
				argSurface();
				setState(745);
				((CmdArgSurfaceContext)_localctx).end = match(T__123);
				}
				break;
			case T__124:
				_localctx = new CmdBankContext(_localctx);
				enterOuterAlt(_localctx, 40);
				{
				setState(747);
				((CmdBankContext)_localctx).open = match(T__124);
				setState(748);
				((CmdBankContext)_localctx).name = ident();
				setState(752);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__142) {
					{
					{
					setState(749);
					set();
					}
					}
					setState(754);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(755);
				list();
				setState(756);
				((CmdBankContext)_localctx).end = match(T__125);
				}
				break;
			case T__126:
				_localctx = new CmdDeleteContext(_localctx);
				enterOuterAlt(_localctx, 41);
				{
				setState(758);
				((CmdDeleteContext)_localctx).open = match(T__126);
				setState(762);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__54) {
					{
					{
					setState(759);
					deleteFace();
					}
					}
					setState(764);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(765);
				((CmdDeleteContext)_localctx).end = match(T__127);
				}
				break;
			case T__128:
				_localctx = new CmdSubdivisionContext(_localctx);
				enterOuterAlt(_localctx, 42);
				{
				setState(766);
				((CmdSubdivisionContext)_localctx).open = match(T__128);
				setState(767);
				((CmdSubdivisionContext)_localctx).name = ident();
				setState(771);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__5) {
					{
					{
					setState(768);
					argSdFlag();
					}
					}
					setState(773);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(777);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__6) {
					{
					{
					setState(774);
					argSdLevel();
					}
					}
					setState(779);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(783);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (((((_la - 14)) & ~0x3f) == 0 && ((1L << (_la - 14)) & ((1L << (T__13 - 14)) | (1L << (T__14 - 14)) | (1L << (T__28 - 14)) | (1L << (T__46 - 14)) | (1L << (T__48 - 14)) | (1L << (T__54 - 14)) | (1L << (T__56 - 14)) | (1L << (T__58 - 14)) | (1L << (T__60 - 14)) | (1L << (T__62 - 14)) | (1L << (T__64 - 14)) | (1L << (T__66 - 14)) | (1L << (T__68 - 14)) | (1L << (T__70 - 14)) | (1L << (T__72 - 14)) | (1L << (T__74 - 14)) | (1L << (T__76 - 14)))) != 0) || ((((_la - 79)) & ~0x3f) == 0 && ((1L << (_la - 79)) & ((1L << (T__78 - 79)) | (1L << (T__80 - 79)) | (1L << (T__82 - 79)) | (1L << (T__84 - 79)) | (1L << (T__86 - 79)) | (1L << (T__88 - 79)) | (1L << (T__90 - 79)) | (1L << (T__92 - 79)) | (1L << (T__94 - 79)) | (1L << (T__96 - 79)) | (1L << (T__98 - 79)) | (1L << (T__100 - 79)) | (1L << (T__103 - 79)) | (1L << (T__105 - 79)) | (1L << (T__108 - 79)) | (1L << (T__110 - 79)) | (1L << (T__112 - 79)) | (1L << (T__114 - 79)) | (1L << (T__116 - 79)) | (1L << (T__118 - 79)) | (1L << (T__120 - 79)) | (1L << (T__122 - 79)) | (1L << (T__124 - 79)) | (1L << (T__126 - 79)) | (1L << (T__128 - 79)) | (1L << (T__130 - 79)) | (1L << (T__132 - 79)) | (1L << (T__134 - 79)) | (1L << (T__136 - 79)) | (1L << (T__138 - 79)) | (1L << (T__140 - 79)))) != 0)) {
					{
					{
					setState(780);
					command();
					}
					}
					setState(785);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(786);
				((CmdSubdivisionContext)_localctx).end = match(T__129);
				}
				break;
			case T__130:
				_localctx = new CmdSharpContext(_localctx);
				enterOuterAlt(_localctx, 43);
				{
				setState(788);
				((CmdSharpContext)_localctx).open = match(T__130);
				setState(789);
				expression(0);
				setState(791); 
				_errHandler.sync(this);
				_la = _input.LA(1);
				do {
					{
					{
					setState(790);
					idList();
					}
					}
					setState(793); 
					_errHandler.sync(this);
					_la = _input.LA(1);
				} while ( _la==LPAREN );
				setState(795);
				((CmdSharpContext)_localctx).end = match(T__131);
				}
				break;
			case T__132:
				_localctx = new CmdOffsetContext(_localctx);
				enterOuterAlt(_localctx, 44);
				{
				setState(797);
				((CmdOffsetContext)_localctx).open = match(T__132);
				setState(798);
				((CmdOffsetContext)_localctx).name = ident();
				setState(802);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__7) {
					{
					{
					setState(799);
					argOffsetFlag();
					}
					}
					setState(804);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(808);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__8) {
					{
					{
					setState(805);
					argHeight();
					}
					}
					setState(810);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(814);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__9) {
					{
					{
					setState(811);
					argWidth();
					}
					}
					setState(816);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(820);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (((((_la - 14)) & ~0x3f) == 0 && ((1L << (_la - 14)) & ((1L << (T__13 - 14)) | (1L << (T__14 - 14)) | (1L << (T__28 - 14)) | (1L << (T__46 - 14)) | (1L << (T__48 - 14)) | (1L << (T__54 - 14)) | (1L << (T__56 - 14)) | (1L << (T__58 - 14)) | (1L << (T__60 - 14)) | (1L << (T__62 - 14)) | (1L << (T__64 - 14)) | (1L << (T__66 - 14)) | (1L << (T__68 - 14)) | (1L << (T__70 - 14)) | (1L << (T__72 - 14)) | (1L << (T__74 - 14)) | (1L << (T__76 - 14)))) != 0) || ((((_la - 79)) & ~0x3f) == 0 && ((1L << (_la - 79)) & ((1L << (T__78 - 79)) | (1L << (T__80 - 79)) | (1L << (T__82 - 79)) | (1L << (T__84 - 79)) | (1L << (T__86 - 79)) | (1L << (T__88 - 79)) | (1L << (T__90 - 79)) | (1L << (T__92 - 79)) | (1L << (T__94 - 79)) | (1L << (T__96 - 79)) | (1L << (T__98 - 79)) | (1L << (T__100 - 79)) | (1L << (T__103 - 79)) | (1L << (T__105 - 79)) | (1L << (T__108 - 79)) | (1L << (T__110 - 79)) | (1L << (T__112 - 79)) | (1L << (T__114 - 79)) | (1L << (T__116 - 79)) | (1L << (T__118 - 79)) | (1L << (T__120 - 79)) | (1L << (T__122 - 79)) | (1L << (T__124 - 79)) | (1L << (T__126 - 79)) | (1L << (T__128 - 79)) | (1L << (T__130 - 79)) | (1L << (T__132 - 79)) | (1L << (T__134 - 79)) | (1L << (T__136 - 79)) | (1L << (T__138 - 79)) | (1L << (T__140 - 79)))) != 0)) {
					{
					{
					setState(817);
					command();
					}
					}
					setState(822);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(823);
				((CmdOffsetContext)_localctx).end = match(T__133);
				}
				break;
			case T__134:
				_localctx = new CmdIncludeContext(_localctx);
				enterOuterAlt(_localctx, 45);
				{
				setState(825);
				((CmdIncludeContext)_localctx).open = match(T__134);
				setState(826);
				((CmdIncludeContext)_localctx).name = ident();
				setState(827);
				((CmdIncludeContext)_localctx).end = match(T__135);
				}
				break;
			case T__136:
				_localctx = new CmdLightContext(_localctx);
				enterOuterAlt(_localctx, 46);
				{
				setState(829);
				((CmdLightContext)_localctx).open = match(T__136);
				setState(830);
				((CmdLightContext)_localctx).name = ident();
				setState(831);
				argLightType();
				setState(832);
				argLightColor();
				setState(833);
				((CmdLightContext)_localctx).end = match(T__137);
				}
				break;
			case T__138:
				_localctx = new CmdCameraContext(_localctx);
				enterOuterAlt(_localctx, 47);
				{
				setState(835);
				((CmdCameraContext)_localctx).open = match(T__138);
				setState(836);
				((CmdCameraContext)_localctx).name = ident();
				setState(837);
				argCameraProjection();
				setState(838);
				argCameraFrustum();
				setState(839);
				((CmdCameraContext)_localctx).end = match(T__139);
				}
				break;
			case T__140:
				_localctx = new CmdViewportContext(_localctx);
				enterOuterAlt(_localctx, 48);
				{
				setState(841);
				((CmdViewportContext)_localctx).open = match(T__140);
				setState(842);
				((CmdViewportContext)_localctx).name = ident();
				setState(843);
				argCameraID();
				setState(844);
				argOrigin();
				setState(845);
				argSize();
				setState(846);
				((CmdViewportContext)_localctx).end = match(T__141);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class SetContext extends ParserRuleContext {
		public Token open;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public SetContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_set; }
	}

	public final SetContext set() throws RecognitionException {
		SetContext _localctx = new SetContext(_ctx, getState());
		enterRule(_localctx, 100, RULE_set);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(850);
			((SetContext)_localctx).open = match(T__142);
			setState(851);
			ident();
			setState(852);
			expression(0);
			setState(853);
			expression(0);
			setState(854);
			expression(0);
			setState(855);
			expression(0);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ListContext extends ParserRuleContext {
		public Token open;
		public IdListContext idList() {
			return getRuleContext(IdListContext.class,0);
		}
		public ListContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_list; }
	}

	public final ListContext list() throws RecognitionException {
		ListContext _localctx = new ListContext(_ctx, getState());
		enterRule(_localctx, 102, RULE_list);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(857);
			((ListContext)_localctx).open = match(T__143);
			setState(858);
			idList();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class DeleteFaceContext extends ParserRuleContext {
		public Token open;
		public Token end;
		public IdentContext ident() {
			return getRuleContext(IdentContext.class,0);
		}
		public DeleteFaceContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_deleteFace; }
	}

	public final DeleteFaceContext deleteFace() throws RecognitionException {
		DeleteFaceContext _localctx = new DeleteFaceContext(_ctx, getState());
		enterRule(_localctx, 104, RULE_deleteFace);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(860);
			((DeleteFaceContext)_localctx).open = match(T__54);
			setState(861);
			ident();
			setState(862);
			((DeleteFaceContext)_localctx).end = match(T__55);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 1:
			return expression_sempred((ExpressionContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean expression_sempred(ExpressionContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 6);
		case 1:
			return precpred(_ctx, 5);
		case 2:
			return precpred(_ctx, 4);
		}
		return true;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3\u00a2\u0363\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\4&\t&\4\'\t\'\4(\t(\4)\t)\4*\t*\4+\t+\4"+
		",\t,\4-\t-\4.\t.\4/\t/\4\60\t\60\4\61\t\61\4\62\t\62\4\63\t\63\4\64\t"+
		"\64\4\65\t\65\4\66\t\66\3\2\7\2n\n\2\f\2\16\2q\13\2\3\2\3\2\3\3\3\3\3"+
		"\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\5\3\u0087"+
		"\n\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\7\3\u0092\n\3\f\3\16\3\u0095"+
		"\13\3\3\4\3\4\5\4\u0099\n\4\3\5\3\5\3\6\3\6\3\6\5\6\u00a0\n\6\3\7\3\7"+
		"\7\7\u00a4\n\7\f\7\16\7\u00a7\13\7\3\7\3\7\3\b\3\b\3\b\3\b\3\b\3\b\3\t"+
		"\3\t\3\n\3\n\3\n\3\13\3\13\3\13\3\f\3\f\3\f\3\r\3\r\3\r\3\16\3\16\3\16"+
		"\3\17\3\17\3\20\3\20\3\21\3\21\3\22\3\22\3\22\3\23\3\23\3\23\3\24\3\24"+
		"\3\24\3\25\3\25\3\25\3\26\3\26\3\26\3\27\3\27\3\27\3\30\3\30\3\30\3\31"+
		"\3\31\3\31\3\32\3\32\3\32\3\33\3\33\3\33\7\33\u00e6\n\33\f\33\16\33\u00e9"+
		"\13\33\3\33\3\33\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34"+
		"\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\3\34\5\34"+
		"\u0105\n\34\3\35\3\35\3\35\3\35\3\35\3\35\3\35\3\35\3\35\3\35\3\36\3\36"+
		"\3\36\3\36\3\36\3\36\3\36\3\37\3\37\3\37\3\37\3\37\3\37\3\37\3 \3 \3 "+
		"\3!\3!\3!\3\"\3\"\3\"\3#\3#\3#\3$\3$\3$\3%\3%\3&\3&\3\'\3\'\3\'\3\'\3"+
		"\'\3\'\3(\3(\3(\3(\3(\3(\3)\3)\3)\3)\3)\3)\3)\3*\3*\3*\3+\3+\3+\3,\3,"+
		"\3,\3-\3-\3-\3.\3.\3/\3/\3\60\3\60\3\61\3\61\3\62\3\62\3\62\3\63\3\63"+
		"\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\7\63\u0169\n\63"+
		"\f\63\16\63\u016c\13\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\7"+
		"\63\u0177\n\63\f\63\16\63\u017a\13\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\7\63\u0185\n\63\f\63\16\63\u0188\13\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\7\63\u0191\n\63\f\63\16\63\u0194\13\63\3\63\7\63\u0197"+
		"\n\63\f\63\16\63\u019a\13\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\7\63\u01a5\n\63\f\63\16\63\u01a8\13\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\7\63\u01b4\n\63\f\63\16\63\u01b7\13\63\3\63\3"+
		"\63\3\63\3\63\3\63\7\63\u01be\n\63\f\63\16\63\u01c1\13\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\7\63\u01f8\n\63\f\63\16\63"+
		"\u01fb\13\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\7\63\u028b\n\63\f\63\16\63\u028e\13\63\3\63\3"+
		"\63\3\63\3\63\3\63\7\63\u0295\n\63\f\63\16\63\u0298\13\63\3\63\3\63\7"+
		"\63\u029c\n\63\f\63\16\63\u029f\13\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\7\63\u02aa\n\63\f\63\16\63\u02ad\13\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\7\63\u02f1\n\63\f\63\16\63\u02f4\13"+
		"\63\3\63\3\63\3\63\3\63\3\63\7\63\u02fb\n\63\f\63\16\63\u02fe\13\63\3"+
		"\63\3\63\3\63\3\63\7\63\u0304\n\63\f\63\16\63\u0307\13\63\3\63\7\63\u030a"+
		"\n\63\f\63\16\63\u030d\13\63\3\63\7\63\u0310\n\63\f\63\16\63\u0313\13"+
		"\63\3\63\3\63\3\63\3\63\3\63\6\63\u031a\n\63\r\63\16\63\u031b\3\63\3\63"+
		"\3\63\3\63\3\63\7\63\u0323\n\63\f\63\16\63\u0326\13\63\3\63\7\63\u0329"+
		"\n\63\f\63\16\63\u032c\13\63\3\63\7\63\u032f\n\63\f\63\16\63\u0332\13"+
		"\63\3\63\7\63\u0335\n\63\f\63\16\63\u0338\13\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3"+
		"\63\3\63\3\63\3\63\3\63\3\63\3\63\5\63\u0353\n\63\3\64\3\64\3\64\3\64"+
		"\3\64\3\64\3\64\3\65\3\65\3\65\3\66\3\66\3\66\3\66\3\66\2\3\4\67\2\4\6"+
		"\b\n\f\16\20\22\24\26\30\32\34\36 \"$&(*,.\60\62\64\668:<>@BDFHJLNPRT"+
		"VXZ\\^`bdfhj\2\4\3\2\u0097\u0098\3\2\u0099\u009a\2\u038f\2o\3\2\2\2\4"+
		"\u0086\3\2\2\2\6\u0098\3\2\2\2\b\u009a\3\2\2\2\n\u009f\3\2\2\2\f\u00a1"+
		"\3\2\2\2\16\u00aa\3\2\2\2\20\u00b0\3\2\2\2\22\u00b2\3\2\2\2\24\u00b5\3"+
		"\2\2\2\26\u00b8\3\2\2\2\30\u00bb\3\2\2\2\32\u00be\3\2\2\2\34\u00c1\3\2"+
		"\2\2\36\u00c3\3\2\2\2 \u00c5\3\2\2\2\"\u00c7\3\2\2\2$\u00ca\3\2\2\2&\u00cd"+
		"\3\2\2\2(\u00d0\3\2\2\2*\u00d3\3\2\2\2,\u00d6\3\2\2\2.\u00d9\3\2\2\2\60"+
		"\u00dc\3\2\2\2\62\u00df\3\2\2\2\64\u00e2\3\2\2\2\66\u0104\3\2\2\28\u0106"+
		"\3\2\2\2:\u0110\3\2\2\2<\u0117\3\2\2\2>\u011e\3\2\2\2@\u0121\3\2\2\2B"+
		"\u0124\3\2\2\2D\u0127\3\2\2\2F\u012a\3\2\2\2H\u012d\3\2\2\2J\u012f\3\2"+
		"\2\2L\u0131\3\2\2\2N\u0137\3\2\2\2P\u013d\3\2\2\2R\u0144\3\2\2\2T\u0147"+
		"\3\2\2\2V\u014a\3\2\2\2X\u014d\3\2\2\2Z\u0150\3\2\2\2\\\u0152\3\2\2\2"+
		"^\u0154\3\2\2\2`\u0156\3\2\2\2b\u0158\3\2\2\2d\u0352\3\2\2\2f\u0354\3"+
		"\2\2\2h\u035b\3\2\2\2j\u035e\3\2\2\2ln\5d\63\2ml\3\2\2\2nq\3\2\2\2om\3"+
		"\2\2\2op\3\2\2\2pr\3\2\2\2qo\3\2\2\2rs\7\2\2\3s\3\3\2\2\2tu\b\3\1\2uv"+
		"\5\n\6\2vw\7\u0095\2\2wx\5\4\3\2xy\7\u0096\2\2y\u0087\3\2\2\2z{\t\2\2"+
		"\2{\u0087\5\4\3\t|}\7\u0095\2\2}~\5\4\3\2~\177\7\u0096\2\2\177\u0087\3"+
		"\2\2\2\u0080\u0081\7\3\2\2\u0081\u0082\7\4\2\2\u0082\u0083\5\4\3\2\u0083"+
		"\u0084\7\5\2\2\u0084\u0087\3\2\2\2\u0085\u0087\5\6\4\2\u0086t\3\2\2\2"+
		"\u0086z\3\2\2\2\u0086|\3\2\2\2\u0086\u0080\3\2\2\2\u0086\u0085\3\2\2\2"+
		"\u0087\u0093\3\2\2\2\u0088\u0089\f\b\2\2\u0089\u008a\7\u009f\2\2\u008a"+
		"\u0092\5\4\3\t\u008b\u008c\f\7\2\2\u008c\u008d\t\3\2\2\u008d\u0092\5\4"+
		"\3\b\u008e\u008f\f\6\2\2\u008f\u0090\t\2\2\2\u0090\u0092\5\4\3\7\u0091"+
		"\u0088\3\2\2\2\u0091\u008b\3\2\2\2\u0091\u008e\3\2\2\2\u0092\u0095\3\2"+
		"\2\2\u0093\u0091\3\2\2\2\u0093\u0094\3\2\2\2\u0094\5\3\2\2\2\u0095\u0093"+
		"\3\2\2\2\u0096\u0099\5\b\5\2\u0097\u0099\5\n\6\2\u0098\u0096\3\2\2\2\u0098"+
		"\u0097\3\2\2\2\u0099\7\3\2\2\2\u009a\u009b\7\u0094\2\2\u009b\t\3\2\2\2"+
		"\u009c\u00a0\7\u0093\2\2\u009d\u009e\7\6\2\2\u009e\u00a0\7\u0093\2\2\u009f"+
		"\u009c\3\2\2\2\u009f\u009d\3\2\2\2\u00a0\13\3\2\2\2\u00a1\u00a5\7\u0095"+
		"\2\2\u00a2\u00a4\5\n\6\2\u00a3\u00a2\3\2\2\2\u00a4\u00a7\3\2\2\2\u00a5"+
		"\u00a3\3\2\2\2\u00a5\u00a6\3\2\2\2\u00a6\u00a8\3\2\2\2\u00a7\u00a5\3\2"+
		"\2\2\u00a8\u00a9\7\u0096\2\2\u00a9\r\3\2\2\2\u00aa\u00ab\7\u0095\2\2\u00ab"+
		"\u00ac\5\4\3\2\u00ac\u00ad\5\4\3\2\u00ad\u00ae\5\4\3\2\u00ae\u00af\7\u0096"+
		"\2\2\u00af\17\3\2\2\2\u00b0\u00b1\7\7\2\2\u00b1\21\3\2\2\2\u00b2\u00b3"+
		"\7\b\2\2\u00b3\u00b4\5\n\6\2\u00b4\23\3\2\2\2\u00b5\u00b6\7\t\2\2\u00b6"+
		"\u00b7\5\4\3\2\u00b7\25\3\2\2\2\u00b8\u00b9\7\n\2\2\u00b9\u00ba\5\n\6"+
		"\2\u00ba\27\3\2\2\2\u00bb\u00bc\7\13\2\2\u00bc\u00bd\5\4\3\2\u00bd\31"+
		"\3\2\2\2\u00be\u00bf\7\f\2\2\u00bf\u00c0\5\4\3\2\u00c0\33\3\2\2\2\u00c1"+
		"\u00c2\7\r\2\2\u00c2\35\3\2\2\2\u00c3\u00c4\7\16\2\2\u00c4\37\3\2\2\2"+
		"\u00c5\u00c6\7\17\2\2\u00c6!\3\2\2\2\u00c7\u00c8\7\20\2\2\u00c8\u00c9"+
		"\5\n\6\2\u00c9#\3\2\2\2\u00ca\u00cb\7\21\2\2\u00cb\u00cc\5\n\6\2\u00cc"+
		"%\3\2\2\2\u00cd\u00ce\7\22\2\2\u00ce\u00cf\5\n\6\2\u00cf\'\3\2\2\2\u00d0"+
		"\u00d1\7\23\2\2\u00d1\u00d2\5\4\3\2\u00d2)\3\2\2\2\u00d3\u00d4\7\24\2"+
		"\2\u00d4\u00d5\5\4\3\2\u00d5+\3\2\2\2\u00d6\u00d7\7\25\2\2\u00d7\u00d8"+
		"\5\n\6\2\u00d8-\3\2\2\2\u00d9\u00da\7\26\2\2\u00da\u00db\5\16\b\2\u00db"+
		"/\3\2\2\2\u00dc\u00dd\7\27\2\2\u00dd\u00de\5\n\6\2\u00de\61\3\2\2\2\u00df"+
		"\u00e0\7\30\2\2\u00e0\u00e1\5\n\6\2\u00e1\63\3\2\2\2\u00e2\u00e3\7\31"+
		"\2\2\u00e3\u00e7\7\u0095\2\2\u00e4\u00e6\5\4\3\2\u00e5\u00e4\3\2\2\2\u00e6"+
		"\u00e9\3\2\2\2\u00e7\u00e5\3\2\2\2\u00e7\u00e8\3\2\2\2\u00e8\u00ea\3\2"+
		"\2\2\u00e9\u00e7\3\2\2\2\u00ea\u00eb\7\u0096\2\2\u00eb\65\3\2\2\2\u00ec"+
		"\u00ed\7\32\2\2\u00ed\u00ee\7\u0095\2\2\u00ee\u00ef\5\4\3\2\u00ef\u00f0"+
		"\5\4\3\2\u00f0\u00f1\5\4\3\2\u00f1\u00f2\7\u0096\2\2\u00f2\u00f3\7\u0095"+
		"\2\2\u00f3\u00f4\5\4\3\2\u00f4\u00f5\7\u0096\2\2\u00f5\u0105\3\2\2\2\u00f6"+
		"\u00f7\7\33\2\2\u00f7\u00f8\7\u0095\2\2\u00f8\u00f9\5\4\3\2\u00f9\u00fa"+
		"\5\4\3\2\u00fa\u00fb\5\4\3\2\u00fb\u00fc\7\u0096\2\2\u00fc\u0105\3\2\2"+
		"\2\u00fd\u00fe\7\34\2\2\u00fe\u00ff\7\u0095\2\2\u00ff\u0100\5\4\3\2\u0100"+
		"\u0101\5\4\3\2\u0101\u0102\5\4\3\2\u0102\u0103\7\u0096\2\2\u0103\u0105"+
		"\3\2\2\2\u0104\u00ec\3\2\2\2\u0104\u00f6\3\2\2\2\u0104\u00fd\3\2\2\2\u0105"+
		"\67\3\2\2\2\u0106\u0107\7\35\2\2\u0107\u0108\7\u0095\2\2\u0108\u0109\5"+
		"\4\3\2\u0109\u010a\5\4\3\2\u010a\u010b\5\4\3\2\u010b\u010c\7\u0096\2\2"+
		"\u010c\u010d\7\u0095\2\2\u010d\u010e\5\4\3\2\u010e\u010f\7\u0096\2\2\u010f"+
		"9\3\2\2\2\u0110\u0111\7\36\2\2\u0111\u0112\7\u0095\2\2\u0112\u0113\5\4"+
		"\3\2\u0113\u0114\5\4\3\2\u0114\u0115\5\4\3\2\u0115\u0116\7\u0096\2\2\u0116"+
		";\3\2\2\2\u0117\u0118\7\26\2\2\u0118\u0119\7\u0095\2\2\u0119\u011a\5\4"+
		"\3\2\u011a\u011b\5\4\3\2\u011b\u011c\5\4\3\2\u011c\u011d\7\u0096\2\2\u011d"+
		"=\3\2\2\2\u011e\u011f\7\32\2\2\u011f\u0120\5\16\b\2\u0120?\3\2\2\2\u0121"+
		"\u0122\7\33\2\2\u0122\u0123\5\16\b\2\u0123A\3\2\2\2\u0124\u0125\7\37\2"+
		"\2\u0125\u0126\5\n\6\2\u0126C\3\2\2\2\u0127\u0128\7 \2\2\u0128\u0129\5"+
		"\4\3\2\u0129E\3\2\2\2\u012a\u012b\7!\2\2\u012b\u012c\5\4\3\2\u012cG\3"+
		"\2\2\2\u012d\u012e\7\"\2\2\u012eI\3\2\2\2\u012f\u0130\7#\2\2\u0130K\3"+
		"\2\2\2\u0131\u0132\7$\2\2\u0132\u0133\7\u0095\2\2\u0133\u0134\5\4\3\2"+
		"\u0134\u0135\5\4\3\2\u0135\u0136\7\u0096\2\2\u0136M\3\2\2\2\u0137\u0138"+
		"\7%\2\2\u0138\u0139\7\u0095\2\2\u0139\u013a\5\4\3\2\u013a\u013b\5\4\3"+
		"\2\u013b\u013c\7\u0096\2\2\u013cO\3\2\2\2\u013d\u013e\7&\2\2\u013e\u013f"+
		"\7\u0095\2\2\u013f\u0140\5\4\3\2\u0140\u0141\5\4\3\2\u0141\u0142\5\4\3"+
		"\2\u0142\u0143\7\u0096\2\2\u0143Q\3\2\2\2\u0144\u0145\7\'\2\2\u0145\u0146"+
		"\5\n\6\2\u0146S\3\2\2\2\u0147\u0148\7(\2\2\u0148\u0149\5\n\6\2\u0149U"+
		"\3\2\2\2\u014a\u014b\7)\2\2\u014b\u014c\5\n\6\2\u014cW\3\2\2\2\u014d\u014e"+
		"\7*\2\2\u014e\u014f\5\n\6\2\u014fY\3\2\2\2\u0150\u0151\7+\2\2\u0151[\3"+
		"\2\2\2\u0152\u0153\7,\2\2\u0153]\3\2\2\2\u0154\u0155\7-\2\2\u0155_\3\2"+
		"\2\2\u0156\u0157\7.\2\2\u0157a\3\2\2\2\u0158\u0159\7/\2\2\u0159\u015a"+
		"\5\4\3\2\u015ac\3\2\2\2\u015b\u015c\7\37\2\2\u015c\u015d\5\n\6\2\u015d"+
		"\u015e\7\u0095\2\2\u015e\u015f\5\4\3\2\u015f\u0160\5\4\3\2\u0160\u0161"+
		"\5\4\3\2\u0161\u0162\7\u0096\2\2\u0162\u0163\7\60\2\2\u0163\u0353\3\2"+
		"\2\2\u0164\u0165\7\61\2\2\u0165\u0166\5\n\6\2\u0166\u016a\5\f\7\2\u0167"+
		"\u0169\5\20\t\2\u0168\u0167\3\2\2\2\u0169\u016c\3\2\2\2\u016a\u0168\3"+
		"\2\2\2\u016a\u016b\3\2\2\2\u016b\u016d\3\2\2\2\u016c\u016a\3\2\2\2\u016d"+
		"\u016e\7\62\2\2\u016e\u0353\3\2\2\2\u016f\u0170\7\63\2\2\u0170\u0171\5"+
		"\n\6\2\u0171\u0172\7\64\2\2\u0172\u0178\5\n\6\2\u0173\u0177\5\36\20\2"+
		"\u0174\u0177\5 \21\2\u0175\u0177\5H%\2\u0176\u0173\3\2\2\2\u0176\u0174"+
		"\3\2\2\2\u0176\u0175\3\2\2\2\u0177\u017a\3\2\2\2\u0178\u0176\3\2\2\2\u0178"+
		"\u0179\3\2\2\2\u0179\u017b\3\2\2\2\u017a\u0178\3\2\2\2\u017b\u017c\7\65"+
		"\2\2\u017c\u017d\7\66\2\2\u017d\u0186\5\n\6\2\u017e\u0185\5D#\2\u017f"+
		"\u0185\5F$\2\u0180\u0185\5J&\2\u0181\u0185\5^\60\2\u0182\u0185\5`\61\2"+
		"\u0183\u0185\5b\62\2\u0184\u017e\3\2\2\2\u0184\u017f\3\2\2\2\u0184\u0180"+
		"\3\2\2\2\u0184\u0181\3\2\2\2\u0184\u0182\3\2\2\2\u0184\u0183\3\2\2\2\u0185"+
		"\u0188\3\2\2\2\u0186\u0184\3\2\2\2\u0186\u0187\3\2\2\2\u0187\u0189\3\2"+
		"\2\2\u0188\u0186\3\2\2\2\u0189\u018a\7\67\2\2\u018a\u018b\78\2\2\u018b"+
		"\u0353\3\2\2\2\u018c\u018d\79\2\2\u018d\u018e\5\n\6\2\u018e\u0192\5\f"+
		"\7\2\u018f\u0191\5\"\22\2\u0190\u018f\3\2\2\2\u0191\u0194\3\2\2\2\u0192"+
		"\u0190\3\2\2\2\u0192\u0193\3\2\2\2\u0193\u0198\3\2\2\2\u0194\u0192\3\2"+
		"\2\2\u0195\u0197\5$\23\2\u0196\u0195\3\2\2\2\u0197\u019a\3\2\2\2\u0198"+
		"\u0196\3\2\2\2\u0198\u0199\3\2\2\2\u0199\u019b\3\2\2\2\u019a\u0198\3\2"+
		"\2\2\u019b\u019c\7:\2\2\u019c\u0353\3\2\2\2\u019d\u019e\7;\2\2\u019e\u019f"+
		"\5\n\6\2\u019f\u01a6\5B\"\2\u01a0\u01a5\5@!\2\u01a1\u01a5\5> \2\u01a2"+
		"\u01a5\5&\24\2\u01a3\u01a5\5H%\2\u01a4\u01a0\3\2\2\2\u01a4\u01a1\3\2\2"+
		"\2\u01a4\u01a2\3\2\2\2\u01a4\u01a3\3\2\2\2\u01a5\u01a8\3\2\2\2\u01a6\u01a4"+
		"\3\2\2\2\u01a6\u01a7\3\2\2\2\u01a7\u01a9\3\2\2\2\u01a8\u01a6\3\2\2\2\u01a9"+
		"\u01aa\7<\2\2\u01aa\u0353\3\2\2\2\u01ab\u01ac\7=\2\2\u01ac\u01ad\5\n\6"+
		"\2\u01ad\u01ae\5\f\7\2\u01ae\u01af\7>\2\2\u01af\u0353\3\2\2\2\u01b0\u01b1"+
		"\7?\2\2\u01b1\u01b5\5\n\6\2\u01b2\u01b4\5d\63\2\u01b3\u01b2\3\2\2\2\u01b4"+
		"\u01b7\3\2\2\2\u01b5\u01b3\3\2\2\2\u01b5\u01b6\3\2\2\2\u01b6\u01b8\3\2"+
		"\2\2\u01b7\u01b5\3\2\2\2\u01b8\u01b9\7@\2\2\u01b9\u0353\3\2\2\2\u01ba"+
		"\u01bb\7A\2\2\u01bb\u01bf\5\n\6\2\u01bc\u01be\5d\63\2\u01bd\u01bc\3\2"+
		"\2\2\u01be\u01c1\3\2\2\2\u01bf\u01bd\3\2\2\2\u01bf\u01c0\3\2\2\2\u01c0"+
		"\u01c2\3\2\2\2\u01c1\u01bf\3\2\2\2\u01c2\u01c3\7B\2\2\u01c3\u0353\3\2"+
		"\2\2\u01c4\u01c5\7C\2\2\u01c5\u01c6\5\n\6\2\u01c6\u01c7\7\u0095\2\2\u01c7"+
		"\u01c8\5\4\3\2\u01c8\u01c9\5\4\3\2\u01c9\u01ca\7\u0096\2\2\u01ca\u01cb"+
		"\7D\2\2\u01cb\u0353\3\2\2\2\u01cc\u01cd\7E\2\2\u01cd\u01ce\5\n\6\2\u01ce"+
		"\u01cf\7\u0095\2\2\u01cf\u01d0\5\4\3\2\u01d0\u01d1\5\4\3\2\u01d1\u01d2"+
		"\5\4\3\2\u01d2\u01d3\7\u0096\2\2\u01d3\u01d4\7F\2\2\u01d4\u0353\3\2\2"+
		"\2\u01d5\u01d6\7G\2\2\u01d6\u01d7\5\n\6\2\u01d7\u01d8\7\u0095\2\2\u01d8"+
		"\u01d9\5\4\3\2\u01d9\u01da\5\4\3\2\u01da\u01db\5\4\3\2\u01db\u01dc\5\4"+
		"\3\2\u01dc\u01dd\5\4\3\2\u01dd\u01de\5\4\3\2\u01de\u01df\7\u0096\2\2\u01df"+
		"\u01e0\7H\2\2\u01e0\u0353\3\2\2\2\u01e1\u01e2\7I\2\2\u01e2\u01e3\5\n\6"+
		"\2\u01e3\u01e4\7\u0095\2\2\u01e4\u01e5\5\4\3\2\u01e5\u01e6\5\4\3\2\u01e6"+
		"\u01e7\5\4\3\2\u01e7\u01e8\5\4\3\2\u01e8\u01e9\5\4\3\2\u01e9\u01ea\5\4"+
		"\3\2\u01ea\u01eb\5\4\3\2\u01eb\u01ec\7\u0096\2\2\u01ec\u01ed\7J\2\2\u01ed"+
		"\u0353\3\2\2\2\u01ee\u01ef\7K\2\2\u01ef\u01f0\5\n\6\2\u01f0\u01f1\7\u0095"+
		"\2\2\u01f1\u01f2\5\4\3\2\u01f2\u01f3\5\4\3\2\u01f3\u01f4\5\4\3\2\u01f4"+
		"\u01f9\5\4\3\2\u01f5\u01f8\5Z.\2\u01f6\u01f8\5\\/\2\u01f7\u01f5\3\2\2"+
		"\2\u01f7\u01f6\3\2\2\2\u01f8\u01fb\3\2\2\2\u01f9\u01f7\3\2\2\2\u01f9\u01fa"+
		"\3\2\2\2\u01fa\u01fc\3\2\2\2\u01fb\u01f9\3\2\2\2\u01fc\u01fd\7\u0096\2"+
		"\2\u01fd\u01fe\7L\2\2\u01fe\u0353\3\2\2\2\u01ff\u0200\7M\2\2\u0200\u0201"+
		"\5\n\6\2\u0201\u0202\7\u0095\2\2\u0202\u0203\5\4\3\2\u0203\u0204\5\4\3"+
		"\2\u0204\u0205\5\4\3\2\u0205\u0206\5\4\3\2\u0206\u0207\5\4\3\2\u0207\u0208"+
		"\5\4\3\2\u0208\u0209\7\u0096\2\2\u0209\u020a\7N\2\2\u020a\u0353\3\2\2"+
		"\2\u020b\u020c\7O\2\2\u020c\u020d\5\n\6\2\u020d\u020e\7\u0095\2\2\u020e"+
		"\u020f\5\4\3\2\u020f\u0210\5\4\3\2\u0210\u0211\5\4\3\2\u0211\u0212\5\4"+
		"\3\2\u0212\u0213\5\4\3\2\u0213\u0214\5\4\3\2\u0214\u0215\5\4\3\2\u0215"+
		"\u0216\7\u0096\2\2\u0216\u0217\7P\2\2\u0217\u0353\3\2\2\2\u0218\u0219"+
		"\7Q\2\2\u0219\u021a\5\n\6\2\u021a\u021b\7\u0095\2\2\u021b\u021c\5\4\3"+
		"\2\u021c\u021d\5\4\3\2\u021d\u021e\5\4\3\2\u021e\u021f\5\4\3\2\u021f\u0220"+
		"\7\u0096\2\2\u0220\u0221\7R\2\2\u0221\u0353\3\2\2\2\u0222\u0223\7S\2\2"+
		"\u0223\u0224\5\n\6\2\u0224\u0225\7\u0095\2\2\u0225\u0226\5\4\3\2\u0226"+
		"\u0227\5\4\3\2\u0227\u0228\5\4\3\2\u0228\u0229\5\4\3\2\u0229\u022a\7\u0096"+
		"\2\2\u022a\u022b\7T\2\2\u022b\u0353\3\2\2\2\u022c\u022d\7U\2\2\u022d\u022e"+
		"\5\n\6\2\u022e\u022f\7\u0095\2\2\u022f\u0230\5\4\3\2\u0230\u0231\5\4\3"+
		"\2\u0231\u0232\5\4\3\2\u0232\u0233\5\4\3\2\u0233\u0234\7\u0096\2\2\u0234"+
		"\u0235\7V\2\2\u0235\u0353\3\2\2\2\u0236\u0237\7W\2\2\u0237\u0238\5\n\6"+
		"\2\u0238\u0239\7\u0095\2\2\u0239\u023a\5\4\3\2\u023a\u023b\5\4\3\2\u023b"+
		"\u023c\5\4\3\2\u023c\u023d\5\4\3\2\u023d\u023e\7\u0096\2\2\u023e\u023f"+
		"\7X\2\2\u023f\u0353\3\2\2\2\u0240\u0241\7Y\2\2\u0241\u0242\5\n\6\2\u0242"+
		"\u0243\7\u0095\2\2\u0243\u0244\5\4\3\2\u0244\u0245\5\4\3\2\u0245\u0246"+
		"\5\4\3\2\u0246\u0247\5\4\3\2\u0247\u0248\5\4\3\2\u0248\u0249\5\4\3\2\u0249"+
		"\u024a\5\4\3\2\u024a\u024b\7\u0096\2\2\u024b\u024c\7Z\2\2\u024c\u0353"+
		"\3\2\2\2\u024d\u024e\7[\2\2\u024e\u024f\5\n\6\2\u024f\u0250\7\u0095\2"+
		"\2\u0250\u0251\5\4\3\2\u0251\u0252\5\4\3\2\u0252\u0253\5\4\3\2\u0253\u0254"+
		"\5\4\3\2\u0254\u0255\5\4\3\2\u0255\u0256\5\4\3\2\u0256\u0257\5\4\3\2\u0257"+
		"\u0258\7\u0096\2\2\u0258\u0259\7\\\2\2\u0259\u0353\3\2\2\2\u025a\u025b"+
		"\7]\2\2\u025b\u025c\5\n\6\2\u025c\u025d\5R*\2\u025d\u025e\7\u0095\2\2"+
		"\u025e\u025f\5\4\3\2\u025f\u0260\5\4\3\2\u0260\u0261\5\4\3\2\u0261\u0262"+
		"\5\4\3\2\u0262\u0263\5\4\3\2\u0263\u0264\5\4\3\2\u0264\u0265\7\u0096\2"+
		"\2\u0265\u0266\7^\2\2\u0266\u0353\3\2\2\2\u0267\u0268\7_\2\2\u0268\u0269"+
		"\5\n\6\2\u0269\u026a\5T+\2\u026a\u026b\5V,\2\u026b\u026c\5X-\2\u026c\u026d"+
		"\7\u0095\2\2\u026d\u026e\5\4\3\2\u026e\u026f\5\4\3\2\u026f\u0270\5\4\3"+
		"\2\u0270\u0271\5\4\3\2\u0271\u0272\5\4\3\2\u0272\u0273\5\4\3\2\u0273\u0274"+
		"\7\u0096\2\2\u0274\u0275\7`\2\2\u0275\u0353\3\2\2\2\u0276\u0277\7a\2\2"+
		"\u0277\u0278\5\n\6\2\u0278\u0279\5R*\2\u0279\u027a\7\u0095\2\2\u027a\u027b"+
		"\5\4\3\2\u027b\u027c\5\4\3\2\u027c\u027d\5\4\3\2\u027d\u027e\5\4\3\2\u027e"+
		"\u027f\5\4\3\2\u027f\u0280\5\4\3\2\u0280\u0281\5\4\3\2\u0281\u0282\5\4"+
		"\3\2\u0282\u0283\5\4\3\2\u0283\u0284\7\u0096\2\2\u0284\u0285\7b\2\2\u0285"+
		"\u0353\3\2\2\2\u0286\u0287\7c\2\2\u0287\u0288\5\n\6\2\u0288\u028c\5\f"+
		"\7\2\u0289\u028b\5(\25\2\u028a\u0289\3\2\2\2\u028b\u028e\3\2\2\2\u028c"+
		"\u028a\3\2\2\2\u028c\u028d\3\2\2\2\u028d\u028f\3\2\2\2\u028e\u028c\3\2"+
		"\2\2\u028f\u0290\7d\2\2\u0290\u0353\3\2\2\2\u0291\u0292\7e\2\2\u0292\u0296"+
		"\5\n\6\2\u0293\u0295\5*\26\2\u0294\u0293\3\2\2\2\u0295\u0298\3\2\2\2\u0296"+
		"\u0294\3\2\2\2\u0296\u0297\3\2\2\2\u0297\u0299\3\2\2\2\u0298\u0296\3\2"+
		"\2\2\u0299\u029d\5\f\7\2\u029a\u029c\5(\25\2\u029b\u029a\3\2\2\2\u029c"+
		"\u029f\3\2\2\2\u029d\u029b\3\2\2\2\u029d\u029e\3\2\2\2\u029e\u02a0\3\2"+
		"\2\2\u029f\u029d\3\2\2\2\u02a0\u02a1\7f\2\2\u02a1\u0353\3\2\2\2\u02a2"+
		"\u02a3\7g\2\2\u02a3\u02a4\5\n\6\2\u02a4\u02ab\5\n\6\2\u02a5\u02aa\5\""+
		"\22\2\u02a6\u02aa\5\66\34\2\u02a7\u02aa\5\34\17\2\u02a8\u02aa\5$\23\2"+
		"\u02a9\u02a5\3\2\2\2\u02a9\u02a6\3\2\2\2\u02a9\u02a7\3\2\2\2\u02a9\u02a8"+
		"\3\2\2\2\u02aa\u02ad\3\2\2\2\u02ab\u02a9\3\2\2\2\u02ab\u02ac\3\2\2\2\u02ac"+
		"\u02ae\3\2\2\2\u02ad\u02ab\3\2\2\2\u02ae\u02af\7h\2\2\u02af\u0353\3\2"+
		"\2\2\u02b0\u02b1\7\20\2\2\u02b1\u02b2\5\n\6\2\u02b2\u02b3\5<\37\2\u02b3"+
		"\u02b4\7i\2\2\u02b4\u0353\3\2\2\2\u02b5\u02b6\7j\2\2\u02b6\u02b7\7\u0095"+
		"\2\2\u02b7\u02b8\5\4\3\2\u02b8\u02b9\5\4\3\2\u02b9\u02ba\5\4\3\2\u02ba"+
		"\u02bb\7\u0096\2\2\u02bb\u02bc\7k\2\2\u02bc\u0353\3\2\2\2\u02bd\u02be"+
		"\7l\2\2\u02be\u02bf\7\u0095\2\2\u02bf\u02c0\5\4\3\2\u02c0\u02c1\5\4\3"+
		"\2\u02c1\u02c2\5\4\3\2\u02c2\u02c3\7\u0096\2\2\u02c3\u02c4\7m\2\2\u02c4"+
		"\u0353\3\2\2\2\u02c5\u02c6\7\21\2\2\u02c6\u02c7\5\n\6\2\u02c7\u02c8\5"+
		"<\37\2\u02c8\u02c9\7n\2\2\u02c9\u0353\3\2\2\2\u02ca\u02cb\7o\2\2\u02cb"+
		"\u02cc\5\n\6\2\u02cc\u02cd\5L\'\2\u02cd\u02ce\5N(\2\u02ce\u02cf\5P)\2"+
		"\u02cf\u02d0\7p\2\2\u02d0\u0353\3\2\2\2\u02d1\u02d2\7q\2\2\u02d2\u02d3"+
		"\5\"\22\2\u02d3\u02d4\7r\2\2\u02d4\u0353\3\2\2\2\u02d5\u02d6\7s\2\2\u02d6"+
		"\u02d7\5\"\22\2\u02d7\u02d8\7t\2\2\u02d8\u0353\3\2\2\2\u02d9\u02da\7u"+
		"\2\2\u02da\u02db\5\"\22\2\u02db\u02dc\7v\2\2\u02dc\u0353\3\2\2\2\u02dd"+
		"\u02de\7w\2\2\u02de\u02df\5\"\22\2\u02df\u02e0\7x\2\2\u02e0\u0353\3\2"+
		"\2\2\u02e1\u02e2\7y\2\2\u02e2\u02e3\5\"\22\2\u02e3\u02e4\7z\2\2\u02e4"+
		"\u0353\3\2\2\2\u02e5\u02e6\7{\2\2\u02e6\u02e7\5\"\22\2\u02e7\u02e8\7|"+
		"\2\2\u02e8\u0353\3\2\2\2\u02e9\u02ea\7}\2\2\u02ea\u02eb\5\"\22\2\u02eb"+
		"\u02ec\7~\2\2\u02ec\u0353\3\2\2\2\u02ed\u02ee\7\177\2\2\u02ee\u02f2\5"+
		"\n\6\2\u02ef\u02f1\5f\64\2\u02f0\u02ef\3\2\2\2\u02f1\u02f4\3\2\2\2\u02f2"+
		"\u02f0\3\2\2\2\u02f2\u02f3\3\2\2\2\u02f3\u02f5\3\2\2\2\u02f4\u02f2\3\2"+
		"\2\2\u02f5\u02f6\5h\65\2\u02f6\u02f7\7\u0080\2\2\u02f7\u0353\3\2\2\2\u02f8"+
		"\u02fc\7\u0081\2\2\u02f9\u02fb\5j\66\2\u02fa\u02f9\3\2\2\2\u02fb\u02fe"+
		"\3\2\2\2\u02fc\u02fa\3\2\2\2\u02fc\u02fd\3\2\2\2\u02fd\u02ff\3\2\2\2\u02fe"+
		"\u02fc\3\2\2\2\u02ff\u0353\7\u0082\2\2\u0300\u0301\7\u0083\2\2\u0301\u0305"+
		"\5\n\6\2\u0302\u0304\5\22\n\2\u0303\u0302\3\2\2\2\u0304\u0307\3\2\2\2"+
		"\u0305\u0303\3\2\2\2\u0305\u0306\3\2\2\2\u0306\u030b\3\2\2\2\u0307\u0305"+
		"\3\2\2\2\u0308\u030a\5\24\13\2\u0309\u0308\3\2\2\2\u030a\u030d\3\2\2\2"+
		"\u030b\u0309\3\2\2\2\u030b\u030c\3\2\2\2\u030c\u0311\3\2\2\2\u030d\u030b"+
		"\3\2\2\2\u030e\u0310\5d\63\2\u030f\u030e\3\2\2\2\u0310\u0313\3\2\2\2\u0311"+
		"\u030f\3\2\2\2\u0311\u0312\3\2\2\2\u0312\u0314\3\2\2\2\u0313\u0311\3\2"+
		"\2\2\u0314\u0315\7\u0084\2\2\u0315\u0353\3\2\2\2\u0316\u0317\7\u0085\2"+
		"\2\u0317\u0319\5\4\3\2\u0318\u031a\5\f\7\2\u0319\u0318\3\2\2\2\u031a\u031b"+
		"\3\2\2\2\u031b\u0319\3\2\2\2\u031b\u031c\3\2\2\2\u031c\u031d\3\2\2\2\u031d"+
		"\u031e\7\u0086\2\2\u031e\u0353\3\2\2\2\u031f\u0320\7\u0087\2\2\u0320\u0324"+
		"\5\n\6\2\u0321\u0323\5\26\f\2\u0322\u0321\3\2\2\2\u0323\u0326\3\2\2\2"+
		"\u0324\u0322\3\2\2\2\u0324\u0325\3\2\2\2\u0325\u032a\3\2\2\2\u0326\u0324"+
		"\3\2\2\2\u0327\u0329\5\30\r\2\u0328\u0327\3\2\2\2\u0329\u032c\3\2\2\2"+
		"\u032a\u0328\3\2\2\2\u032a\u032b\3\2\2\2\u032b\u0330\3\2\2\2\u032c\u032a"+
		"\3\2\2\2\u032d\u032f\5\32\16\2\u032e\u032d\3\2\2\2\u032f\u0332\3\2\2\2"+
		"\u0330\u032e\3\2\2\2\u0330\u0331\3\2\2\2\u0331\u0336\3\2\2\2\u0332\u0330"+
		"\3\2\2\2\u0333\u0335\5d\63\2\u0334\u0333\3\2\2\2\u0335\u0338\3\2\2\2\u0336"+
		"\u0334\3\2\2\2\u0336\u0337\3\2\2\2\u0337\u0339\3\2\2\2\u0338\u0336\3\2"+
		"\2\2\u0339\u033a\7\u0088\2\2\u033a\u0353\3\2\2\2\u033b\u033c\7\u0089\2"+
		"\2\u033c\u033d\5\n\6\2\u033d\u033e\7\u008a\2\2\u033e\u0353\3\2\2\2\u033f"+
		"\u0340\7\u008b\2\2\u0340\u0341\5\n\6\2\u0341\u0342\5,\27\2\u0342\u0343"+
		"\5.\30\2\u0343\u0344\7\u008c\2\2\u0344\u0353\3\2\2\2\u0345\u0346\7\u008d"+
		"\2\2\u0346\u0347\5\n\6\2\u0347\u0348\5\60\31\2\u0348\u0349\5\64\33\2\u0349"+
		"\u034a\7\u008e\2\2\u034a\u0353\3\2\2\2\u034b\u034c\7\u008f\2\2\u034c\u034d"+
		"\5\n\6\2\u034d\u034e\5\62\32\2\u034e\u034f\5L\'\2\u034f\u0350\5N(\2\u0350"+
		"\u0351\7\u0090\2\2\u0351\u0353\3\2\2\2\u0352\u015b\3\2\2\2\u0352\u0164"+
		"\3\2\2\2\u0352\u016f\3\2\2\2\u0352\u018c\3\2\2\2\u0352\u019d\3\2\2\2\u0352"+
		"\u01ab\3\2\2\2\u0352\u01b0\3\2\2\2\u0352\u01ba\3\2\2\2\u0352\u01c4\3\2"+
		"\2\2\u0352\u01cc\3\2\2\2\u0352\u01d5\3\2\2\2\u0352\u01e1\3\2\2\2\u0352"+
		"\u01ee\3\2\2\2\u0352\u01ff\3\2\2\2\u0352\u020b\3\2\2\2\u0352\u0218\3\2"+
		"\2\2\u0352\u0222\3\2\2\2\u0352\u022c\3\2\2\2\u0352\u0236\3\2\2\2\u0352"+
		"\u0240\3\2\2\2\u0352\u024d\3\2\2\2\u0352\u025a\3\2\2\2\u0352\u0267\3\2"+
		"\2\2\u0352\u0276\3\2\2\2\u0352\u0286\3\2\2\2\u0352\u0291\3\2\2\2\u0352"+
		"\u02a2\3\2\2\2\u0352\u02b0\3\2\2\2\u0352\u02b5\3\2\2\2\u0352\u02bd\3\2"+
		"\2\2\u0352\u02c5\3\2\2\2\u0352\u02ca\3\2\2\2\u0352\u02d1\3\2\2\2\u0352"+
		"\u02d5\3\2\2\2\u0352\u02d9\3\2\2\2\u0352\u02dd\3\2\2\2\u0352\u02e1\3\2"+
		"\2\2\u0352\u02e5\3\2\2\2\u0352\u02e9\3\2\2\2\u0352\u02ed\3\2\2\2\u0352"+
		"\u02f8\3\2\2\2\u0352\u0300\3\2\2\2\u0352\u0316\3\2\2\2\u0352\u031f\3\2"+
		"\2\2\u0352\u033b\3\2\2\2\u0352\u033f\3\2\2\2\u0352\u0345\3\2\2\2\u0352"+
		"\u034b\3\2\2\2\u0353e\3\2\2\2\u0354\u0355\7\u0091\2\2\u0355\u0356\5\n"+
		"\6\2\u0356\u0357\5\4\3\2\u0357\u0358\5\4\3\2\u0358\u0359\5\4\3\2\u0359"+
		"\u035a\5\4\3\2\u035ag\3\2\2\2\u035b\u035c\7\u0092\2\2\u035c\u035d\5\f"+
		"\7\2\u035di\3\2\2\2\u035e\u035f\79\2\2\u035f\u0360\5\n\6\2\u0360\u0361"+
		"\7:\2\2\u0361k\3\2\2\2(o\u0086\u0091\u0093\u0098\u009f\u00a5\u00e7\u0104"+
		"\u016a\u0176\u0178\u0184\u0186\u0192\u0198\u01a4\u01a6\u01b5\u01bf\u01f7"+
		"\u01f9\u028c\u0296\u029d\u02a9\u02ab\u02f2\u02fc\u0305\u030b\u0311\u031b"+
		"\u0324\u032a\u0330\u0336\u0352";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}