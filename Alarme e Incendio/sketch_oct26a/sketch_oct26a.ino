#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <Arduino_JSON.h>

#define PIN_PRESENCA 14
#define PIN_BUZZER 17
#define PIN_FOGO 26
#define PIN_FONTE 16

const char* ssid = "STR inacio";
const char* password = "str12345";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
JSONVar readings;
int ativacaoAlarme = 1;
int ativacaoIncendio = 1;

// variáveis de tempo
unsigned long lastTime = 0;
unsigned long timerDelay = 500;

String ultimoResultado = " ";

char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        *{
            font-family: Arial, Helvetica, sans-serif;
        }
        html{
            justify-content: center;
            align-items: center;
        }
        body{
            margin: 0px;
            padding: 0 px;
            background-color: #E5F1FF;;
        }
        #sobre{
            display: none;
            margin: 10px;
        }
        #logo{
            margin: 10px;
            display: flex; 
            align-items: center; 
            width: 30%;
        }
        nav{
            display: flex;
            margin: 0px;
            justify-content: space-between;
            background-color: #04328C;
            padding-left: 5vw;
            padding-right: 5vw;
            height: min-content;
            color: #FFF;
        }
        .list{
            display: flex;
            list-style: none;
            align-items: center;
        }
        .linksAbas{
            text-decoration: none;
            font-size: 20px;
            margin-left: 2vw;
            margin-right: 2vw;
        }
        .links{
            background-color: transparent; 
            font-size: 1.2rem;
            border-width: 0px;
            font-weight: bold;
            color: #FFF;
        }
        .links:hover, .links::selection{
            text-decoration: underline;
            color: white;
            cursor: pointer;
        }
        #cartoes{
            display: flex;
            flex-direction: column;
            align-items: center;
            width: 100%;
        }
        .row-1{
            padding-right: 1%;
            padding-left: 1%;
            display: flex;
            justify-content: space-between;
            padding-bottom: 2em;
            width: 80%;
        }
        .row-2{
            padding-right: 1%;
            padding-left: 1%;
            display: flex;
            justify-content: space-between;
            padding-bottom: 2em;
            width: 40%;
        }
        .cartao{
            border-radius: 13px;
            background: #FFF;
            box-shadow: 0px 4px 4px 0px rgba(0, 0, 0, 0.25);
            width: 100%;
            height: 20em;
            margin-left: 3%;
            margin-right: 3%;
            margin-top: 5%;
            display: flex;
            justify-content: center;
            flex-direction: column;
            text-align: center;
        }

        .titulo-card{
            display: flex;
            background: #04328C;
            box-shadow: 0px 4px 4px 0px rgba(0, 0, 0, 0.25);
            width: 100%;
            margin-bottom: auto;
            margin-top: 0;
            border-radius: 13px 13px 0 0;
            color: #FFF;
            font-size: 12pt;
        }

        .sirene{
            width: 50px;
            height: 50px;
            margin: 0 30px 0 30px;
        }
        .cartao>*{
            display: flex;
            justify-content: center;
            align-items: center;
            margin-bottom: auto;
        }
        .header-cartao > *{
            display: inline;
            margin-right: 0.5em;
            margin-left: 0.5em;
        }
        .on-off-button{
            display: flex;
            background-color: #04328C; 
            color: white;
            font-size: 1.5rem;
            border-width: 0px;
            border-radius: 5px;
            font-weight: bold;
            padding: 9px;
        }
        .situacao{
            font-size: 25pt;
            font-weight: bold;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            border: none;
        }
        th {
            background-color: #04328C;
            color: white;
            padding: 15px;
            text-align: center;
            border: none;
        }
        td {
            padding: 15px;
            text-align: center;
            border: none;
        }
        tr:nth-child(even) {background-color: #f2f2f2;}

        .cx-tabela {
            background-color: #FFF;
            margin: 50px 0 50px 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: min-content;
            width: 80%;
        }
        #home{
            display: grid;
            justify-items: center;
        }
        @media screen and (max-width: 522px) {
            #logo{
                width: max-content;
            }
            nav{
                font-size: 8pt;
            }
            .situacao{
                text-align: center;
                font-size: 12pt;
                font-weight: bold;
            }
            .titulo-card{
                font-size: 7pt;
            }
            #cartoes{
                flex-direction: column;
                align-items: center;
            }
            .cartao {
                padding: 5px;
                text-align: center;
                margin-top: 10px;
                margin-bottom: 10px;
                height: 15em;
            }
            .row-1{
            width: 100%;
            }
            .row-2{
            width: 50%;
            }
        }
    </style>
    <title>Monitoramento</title>
</head>
<body>
    <nav>
        <div id="logo">
            <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32" style="width: 25%;">
                <defs>
                    <style>
                        .cls-1 {
                            fill: #d8e1ef;
                        }
            
                        .cls-2 {
                            fill: #0593ff;
                        }
            
                        .cls-3 {
                            fill: #0e6ae0;
                        }
                    </style>
                </defs>
                <title />
                <g data-name="Home Internet" id="Home_Internet">
                    <path class="cls-1"
                        d="M26.45,7.11l-10-5a1,1,0,0,0-.9,0l-10,5A1,1,0,0,0,5,8V20a3.12,3.12,0,0,0,3.22,3H23.78A3.12,3.12,0,0,0,27,20V8A1,1,0,0,0,26.45,7.11Z" />
                    <path class="cls-2"
                        d="M28,10a.93.93,0,0,1-.45-.11L16,4.12,4.45,9.89a1,1,0,0,1-.9-1.78l12-6a1,1,0,0,1,.9,0l12,6a1,1,0,0,1,.44,1.34A1,1,0,0,1,28,10Z" />
                    <path class="cls-3" d="M14,28H3a1,1,0,0,1,0-2H14a1,1,0,0,1,0,2Z" />
                    <path class="cls-3" d="M29,28H18a1,1,0,0,1,0-2H29a1,1,0,0,1,0,2Z" />
                    <path class="cls-2"
                        d="M22,11a2,2,0,0,0-2,2,2.16,2.16,0,0,0,.07.51l-.78.78A1,1,0,0,0,19,15v8h2V15.41l.49-.48A2.09,2.09,0,0,0,22,15a2,2,0,0,0,0-4Z" />
                    <path class="cls-2"
                        d="M13,23V15a1,1,0,0,0-.29-.71l-.78-.78A2.16,2.16,0,0,0,12,13a2,2,0,1,0-2,2,2.09,2.09,0,0,0,.51-.07l.49.48V23Z" />
                    <path class="cls-2" d="M17,24.18V10.72a2,2,0,1,0-2,0V24.18a3,3,0,1,0,2,0Z" />
                </g>
            </svg>
            <h1 style=" margin-left: 15px;">SMART HOUSE</h1>
        </div>
        <ul class="list">
            <li class="linksAbas">
                <button class="links" id="botao-home">HOME</button>        
            </li>
            <li class="linksAbas">
                <button class="links" id="botao-sobre">SOBRE</button>
            </li>
        </ul>
    </nav>
    <div id="home">
        <div id="cartoes">
            <div class="row-1">
                <div class="cartao" id="alarme">
                    <div class="titulo-card">
                        <h2>ALARME</h2>
                
                    </div>
                    <div>
                        <div>
                            <div class="situacao" id="alarme-texto" style=" margin:10px ;">Sem detecção</div>
                        </div>
                        <div>
                            <svg id="sirene-desativada" style="width: 50px; margin:10px ;" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:cc="http://creativecommons.org/ns#"
                                xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:svg="http://www.w3.org/2000/svg"
                                xmlns="http://www.w3.org/2000/svg" xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
                                xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape" inkscape:export-ydpi="768" inkscape:export-xdpi="768"
                                inkscape:export-filename="E:\S  Y  N  T  X  Y\Dokument Desain\CARLOS\Files\Beginner - Security And Crime - 48x\Beginner - Security And Crime - Expanded - 48px\Export\10. Siren.png"
                                width="48pt" height="48pt" viewBox="0 0 16.933334 16.933333" version="1.1" id="svg979"
                                inkscape:version="1.0 (4035a4fb49, 2020-05-01)" sodipodi:docname="10. Siren.svg">
                                <defs id="defs973" />
                                <sodipodi:namedview id="base" pagecolor="#ffffff" bordercolor="#666666" borderopacity="1.0"
                                    inkscape:pageopacity="0.0" inkscape:pageshadow="2" inkscape:zoom="6.0435533" inkscape:cx="27.659409"
                                    inkscape:cy="30.473392" inkscape:document-units="pt" inkscape:current-layer="layer1" showgrid="false" units="pt"
                                    inkscape:window-width="1366" inkscape:window-height="715" inkscape:window-x="-8" inkscape:window-y="-8"
                                    inkscape:window-maximized="1" inkscape:document-rotation="0" inkscape:snap-bbox="true"
                                    inkscape:bbox-nodes="true" inkscape:snap-nodes="false">
                                    <inkscape:grid type="xygrid" id="grid1526" />
                                    <sodipodi:guide position="0.97013909,15.963209" orientation="56.666615,0" id="guide1528"
                                        inkscape:locked="false" />
                                    <sodipodi:guide position="0.97013909,0.97016731" orientation="0,56.666667" id="guide1530"
                                        inkscape:locked="false" />
                                    <sodipodi:guide position="15.963195,0.97016731" orientation="-56.666615,0" id="guide1532"
                                        inkscape:locked="false" />
                                    <sodipodi:guide position="15.963195,15.963209" orientation="0,-56.666667" id="guide1534"
                                        inkscape:locked="false" />
                                </sodipodi:namedview>
                                <metadata id="metadata976">
                                    <rdf:RDF>
                                        <cc:Work rdf:about="">
                                            <dc:format>image/svg+xml</dc:format>
                                            <dc:type rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
                                            <dc:title />
                                        </cc:Work>
                                    </rdf:RDF>
                                </metadata>
                                <g inkscape:label="Layer 1" inkscape:groupmode="layer" id="layer1" transform="translate(0,-280.06668)">
                                    <path
                                        sodipodi:nodetypes="cccccccccccccccccscccccssssscssssssccccsssscccsccsccsscscssccsccscccsccccccccsccsccsccccccccccccccccssccssccccccccccccccccccccccccccccccssssssssssc"
                                        d="m 1.5279724,282.28391 c -0.00694,-10e-6 -0.013875,2e-4 -0.020781,6.4e-4 -0.099661,0.007 -0.1904277,0.0599 -0.2457937,0.14306 -0.099093,0.14796 -0.059606,0.34821 0.088235,0.44746 l 1.9367236,1.29137 c 0.1486079,0.0985 0.3490226,
                                        0.0579 0.44747,-0.0907 0.098726,-0.14845 0.058369,-0.34882 -0.090127,-0.44747 l -1.9367235,-1.28883 c -0.052865,-0.0359 -0.1151522,-0.0551 -0.1789886,-0.0555 z m 13.8778636,6.4e-4 c -0.06442,-2e-4 -0.12741,0.0189 -0.180879,0.0549 l -1.936724,
                                        1.28884 c -0.14784,0.0993 -0.187327,0.29951 -0.08824,0.44746 0.09848,0.14911 0.299368,0.18979 0.4481,0.0907 l 1.936723,-1.29137 c 0.147623,-0.0994 0.186822,-0.29968 0.0876,-0.44747 -0.05961,-0.0889 -0.159492,-0.14257 -0.266591,-0.14306 z m -8.9563259,
                                        0.23571 c -0.9058858,0 -1.6811527,0.66282 -1.8484922,1.56487 l 0.005,-0.0233 -1.6153024,6.10953 c -0.00159,0.009 -0.00329,0.016 -0.00439,0.0239 -0.070041,0.37758 0.022428,0.74251 0.2142801,1.03863 H 2.657341 c -0.2689008,
                                        0 -0.5621868,0.11074 -0.82435,0.35483 -0.2621665,0.24403 -0.4644883,0.63653 -0.4644883,1.14578 v 0.54766 c 0,0.50399 0.1912118,0.89706 0.4506227,1.14326 0.2594107,0.2462 0.5626288,0.35735 0.8382156,0.35735 3.8721911,10e-6 7.745005,10e-6 11.617196,
                                        0 0.275588,0 0.58133,-0.11111 0.84074,-0.35735 0.259411,-0.2462 0.449991,-0.63927 0.449991,-1.14326 v -0.54766 c 0,-0.50925 -0.202323,-0.90175 -0.464489,-1.14578 -0.262163,-0.24403 -0.55734,-0.35483 -0.826242,-0.35483 h -0.540744 c 0.191851,
                                        -0.29612 0.284323,-0.66105 0.214281,-1.03863 -0.0016,-0.004 -0.0033,-0.009 -0.005,-0.0122 l -1.366976,-6.09819 c -0.167341,-0.90204 -0.945131,-1.56488 -1.851013,-1.56488 z m 0,0.64535 h 4.2755409 c 0.591923,0 1.102179,0.42993 1.2151,
                                        1.03864 6.45e-4,0.004 0.0016,0.009 0.0026,0.0122 l 1.369508,6.09566 c 0.09053,0.488 -0.262072,0.92205 -0.731079,0.92205 H 9.4343547 v -2.90603 H 10.07972 c 0.352604,0 0.645359,-0.29089 0.645365,-0.64348 v -0.96931 c -6e-6,-0.3526 -0.292773,
                                        -0.64536 -0.645365,-0.64536 H 9.4343547 c -0.00293,0 -0.00585,5.8e-4 -0.00878,6.4e-4 -0.04421,-0.49097 -0.456856,-0.88233 -0.9585915,-0.88233 -0.5017356,0 -0.916034,0.39136 -0.9604831,0.88233 -0.00293,-3e-5 -0.00585,-6.4e-4 -0.00878,
                                        -6.4e-4 H 6.8548727 c -0.3525911,0 -0.6459907,0.29276 -0.6459968,0.64536 v 0.96931 c 6.1e-6,0.35259 0.2933886,0.64348 0.6459968,0.64348 h 0.6428417 v 2.90603 H 4.3534527 c -0.4690055,0 -0.8216038,-0.43405 -0.7310751,-0.92205 l 1.6077385,
                                        -6.08369 c 0.00195,-0.007 0.00366,-0.0159 0.005,-0.0239 0.1129215,-0.60871 0.6225441,-1.03864 1.2144693,-1.03864 z m 0.7266639,0.64601 c -0.9017929,0 -1.6797514,0.6431 -1.8484886,1.52896 l -0.7361218,2.72766 c -0.046052,0.17232 0.056418,
                                        0.34931 0.2287786,0.39516 0.1715447,0.0462 0.3481872,-0.0549 0.3951596,-0.22624 l 0.7411599,-2.751 c 0.00159,-0.009 0.00329,-0.016 0.00439,-0.0239 0.111519,-0.58549 0.6190817,-1.00524 1.2150987,-1.00524 0.1782108,-7e-4 0.3221172,-0.14573 0.3214208,
                                        -0.32393 -6.451e-4,-0.17723 -0.1441905,-0.32074 -0.3214208,-0.32142 z m 1.1287593,0 c -0.1772304,6.8e-4 -0.3207331,0.14419 -0.3214233,0.32142 -6.452e-4,0.17821 0.1432087,0.32324 0.3214233,0.32393 h 0.4840234 c 0.1791988,7.2e-4 0.3246418,-0.14474 0.3239406,
                                        -0.32393 -6.452e-4,-0.17822 -0.1457297,-0.32212 -0.3239406,-0.32142 z m -6.8406132,1.37644 c -0.1784596,3.5e-4 -0.3227503,0.14548 -0.3220533,0.32394 3.227e-4,0.17772 0.1443326,0.3217 0.3220533,0.32205 h 1.678955 c 0.1774755,-6.7e-4 0.3210782,-0.14457 0.3214233,
                                        -0.32205 6.452e-4,-0.17822 -0.1432087,-0.32325 -0.3214233,-0.32394 z m 12.3268429,0 c -0.17846,3.5e-4 -0.32275,0.14548 -0.322053,0.32394 3.22e-4,0.17772 0.144332,0.3217 0.322053,0.32205 h 1.678959 c 0.177475,-7.1e-4 0.321074,-0.14457 0.32142,-0.32205 6.46e-4,
                                        -0.17822 -0.14321,-0.32325 -0.32142,-0.32394 z m -5.3242589,0.64599 c 0.1820319,0 0.3220526,0.13938 0.3220526,0.32142 v 5.07847 H 8.1429599 v -5.07847 c 0,-0.18204 0.1419124,-0.32142 0.3239442,-0.32142 z m -1.6121497,0.88107 h 0.6428405 v 0.96931 H 6.8547544 Z m 2.5795674,
                                        0 h 0.6453652 v 0.96931 H 9.4343218 Z m 4.6631382,0.0857 c -0.109653,0.006 -0.208902,0.0667 -0.263441,0.16197 -0.08876,0.15455 -0.03543,0.35178 0.119117,0.44054 l 1.526439,0.8817 c 0.154049,0.089 0.351137,0.0366 0.440539,-0.1172 0.08887,
                                        -0.15437 0.03583,-0.35157 -0.118483,-0.44054 l -1.527069,-0.88106 c -0.04551,-0.0272 -0.09703,-0.0428 -0.149998,-0.0454 -0.009,-3.9e-4 -0.01807,-3.9e-4 -0.02711,0 z m -11.2913623,0.001 c -0.0059,2.6e-4 -0.011781,6.8e-4 -0.017651,10e-4 -0.046276,0.005 -0.090993,
                                        0.0193 -0.1310896,0.0428 l -1.5270683,0.88108 c -0.15363551,0.0896 -0.20578419,0.28665 -0.116595,0.44054 0.089403,0.15386 0.2864904,0.2063 0.4405389,0.1172 l 1.5270691,-0.88159 c 0.1545484,-0.0888 0.2078773,-0.286 0.119117,-0.44053 -0.060382,-0.1044 -0.1738972,
                                        -0.16633 -0.2943224,-0.16072 z m -0.1487373,5.07721 c 3.8764183,0.005 7.7405736,0 11.6171966,0 0.05378,0 0.24452,0.0494 0.385705,0.18088 0.141187,0.13141 0.25966,0.3272 0.25966,0.67435 v 0.54768 c 0,0.34988 -0.114702,0.54827 -0.250204,0.67688 -0.135507,0.1286 -0.314326,
                                        0.17835 -0.395161,0.17835 -3.872188,10e-6 -7.7450091,10e-6 -11.6171966,0 -0.080835,0 -0.2596528,-0.0498 -0.3951567,-0.17835 -0.1355072,-0.12861 -0.2476845,-0.327 -0.2476845,-0.67688 v -0.54768 c 0,-0.34715 0.1184731,-0.54294 0.259656,-0.67435 0.1411864,-0.13142 0.3293973,
                                        -0.18088 0.3831852,-0.18088 z"
                                        style=" color:#04328C;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:medium;line-height:normal;font-family:sans-serif;font-variant-ligatures:normal;font-variant-position:normal;font-variant-caps:normal;font-variant-numeric:normal;
                                        font-variant-alternates:normal;font-variant-east-asian:normal;font-feature-settings:normal;font-variation-settings:normal;text-indent:0;text-align:start;text-decoration:none;text-decoration-line:none;text-decoration-style:solid;text-decoration-color:#04328C;letter-spacing:normal;
                                        word-spacing:normal;text-transform:none;writing-mode:lr-tb;direction:ltr;text-orientation:mixed;dominant-baseline:auto;baseline-shift:baseline;text-anchor:start;white-space:normal;shape-padding:0;shape-margin:0;inline-size:0;clip-rule:nonzero;display:inline;overflow:visible;visibility:visible;
                                        opacity:1;isolation:auto;mix-blend-mode:normal;color-interpolation:sRGB;color-interpolation-filters:linearRGB;solid-color:#000000;solid-opacity:1;vector-effect:none;fill:#04328C;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:0.645365;stroke-linecap:round;stroke-linejoin:round;
                                        stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;color-rendering:auto;image-rendering:auto;shape-rendering:auto;text-rendering:auto;enable-background:accumulate;stop-color:#04328C;stop-opacity:1"
                                        id="path983" />
                                </g>
                            </svg>

                        </div>
                    </div>
                    <div>
                        <button id="alarme-button" class="on-off-button">LIGADO</button>
                    </div>
                
                </div>
                <div class="cartao" id="incendio">
                    <div class="titulo-card">
                        <h2>INCÊNDIO</h2>
                
                    </div>
                    <div>
                        <div>
                            <div class="situacao" id="incendio-texto" style=" margin:10px ;">Sem detecção</div>
                        </div>
                        <div>
                            <svg id="fogo-desativado" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1"
                                id="mdi-fire" width="60" height="60" viewBox="0 0 24 24">
                                <path style="stop-color:#04328C; fill:#04328C; color: #04328C; text-decoration-color:#04328C"
                                    d="M17.55,11.2C17.32,10.9 17.05,10.64 16.79,10.38C16.14,9.78 15.39,9.35 14.76,8.72C13.3,7.26 13,4.85 13.91,3C13,3.23 12.16,3.75 11.46,4.32C8.92,6.4 7.92,10.07 9.12,13.22C9.16,13.32 9.2,13.42 9.2,13.55C9.2,13.77 9.05,13.97 8.85,14.05C8.63,14.15 8.39,14.09 8.21,13.93C8.15,13.88 8.11,13.83 8.06,13.76C6.96,12.33 6.78,10.28 7.53,8.64C5.89,10 5,12.3 5.14,
                                    14.47C5.18,14.97 5.24,15.47 5.41,15.97C5.55,16.57 5.81,17.17 6.13,17.7C7.17,19.43 9,20.67 10.97,20.92C13.07,21.19 15.32,20.8 16.93,19.32C18.73,17.66 19.38,15 18.43,12.72L18.3,12.46C18.1,12 17.83,11.59 17.5,11.21L17.55,11.2M14.45,17.5C14.17,17.74 13.72,18 13.37,18.1C12.27,18.5 11.17,17.94 10.5,17.28C11.69,17 12.39,16.12 12.59,15.23C12.76,14.43 12.45,
                                    13.77 12.32,13C12.2,12.26 12.22,11.63 12.5,10.94C12.67,11.32 12.87,11.7 13.1,12C13.86,13 15.05,13.44 15.3,14.8C15.34,14.94 15.36,15.08 15.36,15.23C15.39,16.05 15.04,16.95 14.44,17.5H14.45Z" />
                            </svg>
                            
                            <svg id="fogo-ativado" style="display: none;" xmlns="http://www.w3.org/2000/svg"
                                xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="mdi-fire" width="60" height="60" viewBox="0 0 24 24">
                                <path style=" stop-color:#f58e08; fill:#f58e08; color: #f58e08; text-decoration-color:#f58e08"
                                    d="M17.55,11.2C17.32,10.9 17.05,10.64 16.79,10.38C16.14,9.78 15.39,9.35 14.76,8.72C13.3,7.26 13,4.85 13.91,3C13,3.23 12.16,3.75 11.46,4.32C8.92,6.4 7.92,10.07 9.12,13.22C9.16,13.32 9.2,13.42 9.2,13.55C9.2,13.77 9.05,13.97 8.85,14.05C8.63,14.15 8.39,14.09 8.21,13.93C8.15,13.88 8.11,13.83 8.06,13.76C6.96,12.33 6.78,10.28 7.53,
                                    8.64C5.89,10 5,12.3 5.14,14.47C5.18,14.97 5.24,15.47 5.41,15.97C5.55,16.57 5.81,17.17 6.13,17.7C7.17,19.43 9,20.67 10.97,20.92C13.07,21.19 15.32,20.8 16.93,19.32C18.73,17.66 19.38,15 18.43,12.72L18.3,12.46C18.1,12 17.83,11.59 17.5,11.21L17.55,11.2M14.45,17.5C14.17,17.74 13.72,18 13.37,18.1C12.27,18.5 11.17,17.94 10.5,17.28C11.69,17 12.39,
                                    16.12 12.59,15.23C12.76,14.43 12.45,13.77 12.32,13C12.2,12.26 12.22,11.63 12.5,10.94C12.67,11.32 12.87,11.7 13.1,12C13.86,13 15.05,13.44 15.3,14.8C15.34,14.94 15.36,15.08 15.36,15.23C15.39,16.05 15.04,16.95 14.44,17.5H14.45Z" />
                            </svg>
                        </div>
                    </div>
                    <div>
                        <button id="incendio-button" class="on-off-button">LIGADO</button>
                    </div>
                
                </div>
            </div>
            <div class="row-2">
                <div class="cartao" id="abastecimento">
                    <div class="titulo-card">
                        <h2>SISTEMA DE ABASTECIMENTO</h2>
            
                    </div>
                    <div style="display: flex; flex-direction: column;">
                        <div id="status-baterias" style="width: 100%; display: flex; justify-content: space-around;">
                            <h2 class="situacao" style="margin: 10px ;">Fonte Primária</h2>
                            <h2 class="situacao" style="margin: 10px ;">Fonte Secundária</h2>
                        </div>
                        <div style="display: flex; width: 100%;">
                            <svg id="primaria-ativada" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="Layer_1" x="0px"
                                y="0px" viewBox="0 0 24 24" enable-background="new 0 0 24 24" xml:space="preserve" width="50%" height="50%">
                                <g>
                                    <path fill="#303C42"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5C8.2236328,0,8,0.2239016,8,0.5006009V3H6   C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12c1.1025391,0,2-0.8980503,2-2.0024033   V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                    <rect x="9" y="1" fill="#F3F3F3" width="6" height="2" />
                                    <path fill="#E6E6E5"
                                        d="M19,22c0,0.5512695-0.4482422,1-1,1H6c-0.5517578,0-1-0.4487305-1-1V5c0-0.5512695,0.4482422-1,1-1h12   c0.5517578,0,1,0.4487305,1,1V22z" />
                                    <path fill="#303C42"
                                        d="M17.5,5h-11C6.2236328,5,6,5.2236328,6,5.5v16C6,21.7763672,6.2236328,22,6.5,22h11   c0.2763672,0,0.5-0.2236328,0.5-0.5v-16C18,5.2236328,17.7763672,5,17.5,5z" />
                                    <rect x="7" y="6" fill="#D1DEF2" width="10" height="3" />
                                    <rect x="7" y="10" fill="#95B8E1" width="10" height="3" />
                                    <rect x="7" y="14" fill="#1B87C9" width="10" height="3" />
                                    <rect x="7" y="18" fill="#00579B" width="10" height="3" />
                                    <rect x="7" y="6" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="10" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="14" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="18" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="6" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="10" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="14" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="18" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <linearGradient id="SVGID_1_" gradientUnits="userSpaceOnUse" x1="2.3201692" y1="8.1399946" x2="22.3201637"
                                        y2="17.4661446">
                                        <stop offset="0" style="stop-color:#FFFFFF;stop-opacity:0.2" />
                                        <stop offset="1" style="stop-color:#FFFFFF;stop-opacity:0" />
                                    </linearGradient>
                                    <path fill="url(#SVGID_1_)"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5   C8.2236328,0,8,0.2239016,8,0.5006009V3H6C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12   c1.1025391,0,2-0.8980503,2-2.0024033V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                            </svg>



                            <svg id="primaria-desativada" style="display:none;" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="Layer_1" x="0px"
                                y="0px" viewBox="0 0 24 24" enable-background="new 0 0 24 24" xml:space="preserve" width="50%" height="50%">
                                <g>
                                    <path fill="#04328C"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5C8.2236328,0,8,0.2239016,8,0.5006009V3H6   C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12c1.1025391,0,2-0.8980503,2-2.0024033   V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                    <rect x="9" y="1" fill="#F3F3F3" width="6" height="2" />
                                    <path fill="#E6E6E5"
                                        d="M19,22c0,0.5512695-0.4482422,1-1,1H6c-0.5517578,0-1-0.4487305-1-1V5c0-0.5512695,0.4482422-1,1-1h12   c0.5517578,0,1,0.4487305,1,1V22z" />
                                    <path fill="#04328C"
                                        d="M17.5,5h-11C6.2236328,5,6,5.2236328,6,5.5v16C6,21.7763672,6.2236328,22,6.5,22h11   c0.2763672,0,0.5-0.2236328,0.5-0.5v-16C18,5.2236328,17.7763672,5,17.5,5z" />
                                    <rect x="7" y="6" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="10" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="14" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="18" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="6" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="10" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="14" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="18" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="6" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="10" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="14" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="18" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <linearGradient id="SVGID_1_" gradientUnits="userSpaceOnUse" x1="2.3201692" y1="8.1399946" x2="22.3201637"
                                        y2="17.4661446">
                                        <stop offset="0" style="stop-color:#FFFFFF;stop-opacity:0.2" />
                                        <stop offset="1" style="stop-color:#FFFFFF;stop-opacity:0" />
                                    </linearGradient>
                                    <path fill="url(#SVGID_1_)"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5   C8.2236328,0,8,0.2239016,8,0.5006009V3H6C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12   c1.1025391,0,2-0.8980503,2-2.0024033V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                            </svg>


                            <svg id="secundaria-ativada" style="display:none ;" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" id="Layer_1" x="0px"
                                y="0px" viewBox="0 0 24 24" enable-background="new 0 0 24 24" xml:space="preserve" width="50%" height="50%">
                                <g>
                                    <path fill="#303C42"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5C8.2236328,0,8,0.2239016,8,0.5006009V3H6   C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12c1.1025391,0,2-0.8980503,2-2.0024033   V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                    <rect x="9" y="1" fill="#F3F3F3" width="6" height="2" />
                                    <path fill="#E6E6E5"
                                        d="M19,22c0,0.5512695-0.4482422,1-1,1H6c-0.5517578,0-1-0.4487305-1-1V5c0-0.5512695,0.4482422-1,1-1h12   c0.5517578,0,1,0.4487305,1,1V22z" />
                                    <path fill="#303C42"
                                        d="M17.5,5h-11C6.2236328,5,6,5.2236328,6,5.5v16C6,21.7763672,6.2236328,22,6.5,22h11   c0.2763672,0,0.5-0.2236328,0.5-0.5v-16C18,5.2236328,17.7763672,5,17.5,5z" />
                                    <rect x="7" y="6" fill="#D1DEF2" width="10" height="3" />
                                    <rect x="7" y="10" fill="#95B8E1" width="10" height="3" />
                                    <rect x="7" y="14" fill="#1B87C9" width="10" height="3" />
                                    <rect x="7" y="18" fill="#00579B" width="10" height="3" />
                                    <rect x="7" y="6" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="10" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="14" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="18" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="6" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="10" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="14" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <rect x="15.5" y="18" opacity="0.1" fill="#010101" width="1.5" height="3" />
                                    <linearGradient id="SVGID_1_" gradientUnits="userSpaceOnUse" x1="2.3201692" y1="8.1399946" x2="22.3201637"
                                        y2="17.4661446">
                                        <stop offset="0" style="stop-color:#FFFFFF;stop-opacity:0.2" />
                                        <stop offset="1" style="stop-color:#FFFFFF;stop-opacity:0" />
                                    </linearGradient>
                                    <path fill="url(#SVGID_1_)"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5   C8.2236328,0,8,0.2239016,8,0.5006009V3H6C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12   c1.1025391,0,2-0.8980503,2-2.0024033V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                            </svg>
                            
                            
                            
                            <svg id="secundaria-desativada" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
                                version="1.1" id="Layer_1" x="0px" y="0px" viewBox="0 0 24 24" enable-background="new 0 0 24 24"
                                xml:space="preserve" width="50%" height="50%">
                                <g>
                                    <path fill="#04328C"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5C8.2236328,0,8,0.2239016,8,0.5006009V3H6   C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12c1.1025391,0,2-0.8980503,2-2.0024033   V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                    <rect x="9" y="1" fill="#F3F3F3" width="6" height="2" />
                                    <path fill="#E6E6E5"
                                        d="M19,22c0,0.5512695-0.4482422,1-1,1H6c-0.5517578,0-1-0.4487305-1-1V5c0-0.5512695,0.4482422-1,1-1h12   c0.5517578,0,1,0.4487305,1,1V22z" />
                                    <path fill="#04328C"
                                        d="M17.5,5h-11C6.2236328,5,6,5.2236328,6,5.5v16C6,21.7763672,6.2236328,22,6.5,22h11   c0.2763672,0,0.5-0.2236328,0.5-0.5v-16C18,5.2236328,17.7763672,5,17.5,5z" />
                                    <rect x="7" y="6" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="10" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="14" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="18" fill="#FFFFFF" width="10" height="3" />
                                    <rect x="7" y="6" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="10" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="14" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="7" y="18" opacity="0.2" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="6" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="10" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="14" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <rect x="15.5" y="18" opacity="0.1" fill="#FFFFFF" width="1.5" height="3" />
                                    <linearGradient id="SVGID_1_" gradientUnits="userSpaceOnUse" x1="2.3201692" y1="8.1399946" x2="22.3201637"
                                        y2="17.4661446">
                                        <stop offset="0" style="stop-color:#FFFFFF;stop-opacity:0.2" />
                                        <stop offset="1" style="stop-color:#FFFFFF;stop-opacity:0" />
                                    </linearGradient>
                                    <path fill="url(#SVGID_1_)"
                                        d="M18,3h-2V0.5006009C16,0.2239016,15.8095703,0,15.5332031,0H8.5   C8.2236328,0,8,0.2239016,8,0.5006009V3H6C4.8974609,3,4,3.8728127,4,4.9771657v17.0204315C4,23.1019497,4.8974609,24,6,24h12   c1.1025391,0,2-0.8980503,2-2.0024033V4.9771657C20,3.8728127,19.1025391,3,18,3z" />
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                                <g>
                                </g>
                            </svg>
                        </div>
                    </div>
                               
                </div>
            </div>
            
        </div>
        <div>
            <button id="limpar-historico" class="on-off-button">LIMPAR HISTORICO</button>
        </div>
        <div class="cx-tabela">
            <table id="historico">
                <tr>
                    <th>Horário</th>
                    <th>Evento</th>
                    <th>Data</th>
                </tr>
            </table>
        </div>
    </div>
    <div id="sobre">
        <h2>SOBRE</h2>
        <p>Site feito na disciplina de Software em Tempo Real (STR) usando um ESP32 e Web Socket para a comunicação cliente-servidor</p>
    </div>
</body>
<Script>
    
    var botaoLimparHis = document.querySelector("#limpar-historico")
    botaoLimparHis.addEventListener('click', function(e){
        var table = document.querySelector('#historico')
        var tam = ((table.rows.length)-1)
        e.preventDefault();
        
        while (tam > 0) {
            table.deleteRow(tam);
            tam--;
        }
    })
    
    
    const primariaAtivada =document.querySelector('#primaria-ativada');
    const primariaDesativada = document.querySelector('#primaria-desativada');
    const secundariaAtivada = document.querySelector('#secundaria-ativada');
    const secundariaDesativada = document.querySelector('#secundaria-desativada');
    const statusBaterias = document.querySelector('#status-baterias').children;
    statusBaterias[0].style.color = "green";
    statusBaterias[1].style.color = "black";
    
    function n(){}

    function alternarBateriaPrimaria() {
        secundariaAtivada.style.display = "none";
        secundariaDesativada.style.display = "block";
        primariaDesativada.style.display = "none";
        primariaAtivada.style.display = "block";

        statusBaterias[0].style.color = "green";
        statusBaterias[1].style.color = "black";
    }

    function alternarBateriaSecundaria(){
        primariaAtivada.style.display = "none";
        primariaDesativada.style.display = "block";
        secundariaDesativada.style.display = "none";
        secundariaAtivada.style.display = "block";

        statusBaterias[1].style.color = "green";
        statusBaterias[0].style.color = "black";
    }

    const botaoIncedio = document.querySelector("#incendio-button");
    botaoIncedio.addEventListener("click", function (e){
        const botaoIncedio = document.querySelector("#incendio-button");
        e.preventDefault();
        if(botaoIncedio.innerHTML == "DESLIGADO"){
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/ativarincendio", true);
            xhttp.send();
        }
        else{
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/desativarincendio", true);
            xhttp.send()
        }
     })
    const botaoAlarme = document.querySelector("#alarme-button");
    botaoAlarme.addEventListener("click", function (e){
        const botaoAlarme = document.querySelector("#alarme-button");
        e.preventDefault();
        if(botaoAlarme.innerHTML == "DESLIGADO"){
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/ativaralarme", true);
            xhttp.send()
        }
        else{
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/desativaralarme", true);
            xhttp.send()
        }
     })
    function clickHome(e) {
        e.preventDefault();
        document.getElementById("home").style.display = "block";
        document.getElementById("sobre").style.display = "none";
    }
    function clickSobre(e) {
        e.preventDefault();
        document.getElementById("home").style.display = "none";
        document.getElementById("sobre").style.display = "block";
    }
    document.getElementById("botao-home").addEventListener("click", clickHome);
    document.getElementById("botao-sobre").addEventListener("click", clickSobre);
    
    function addCard(nomeEvento) {
        // Seleciona a tabela
        var table = document.querySelector('#historico');
        // Insere uma nova linha no final da tabela
        var newRow = table.insertRow(1);
        // Insere novas células na linha
        var hora = newRow.insertCell(0);
        var evento = newRow.insertCell(1);
        var data = newRow.insertCell(2);
        var now = new Date();
        // Adiciona o conteúdo às células
        hora.innerHTML = now.toLocaleTimeString();
        evento.innerHTML = nomeEvento;
        data.innerHTML =  now.toLocaleDateString();
    }
    
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    // Init web socket when the page loads
    window.addEventListener('load', onload);
    var alarme = false;
    var incendio = false;
    function onload(event) {
        initWebSocket();
    }

    function getReadings(){
        websocket.send("getReadings");
    }

    function initWebSocket() {
        console.log('Tentando abrir conecção com o Web Socket');
        websocket = new WebSocket(gateway);
        websocket.onopen = onOpen;
        websocket.onclose = onClose;
        websocket.onmessage = onMessage;
    }

    //When websocket is established, call the getReadings() function
    function onOpen(event) {
        console.log('Connection opened');
        getReadings();
    }

    function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
    }

    function ativarAlarme(){
        addCard("Alarme iniciado")
        const elemento = document.getElementById("alarme-texto")
        elemento.style.color = "#ff0000"
        elemento.innerHTML = "ALARME ATIVADO"
    }
    
    function ativarIncendio(){
        const elemento = document.getElementById("incendio-texto")
        elemento.style.color = "#ff0000"
        elemento.innerHTML = "INCÊNDIO DETECTADO"
        addCard("Incendio iniciado")
    }

    function desativarAlarme(){
        const elemento = document.getElementById("alarme-texto")
        elemento.style.color = "#000000"
        elemento.innerHTML = "Sem detecção"
        addCard("Alarme terminado")
    }
    
    function desativarIncendio(){
        const elemento = document.getElementById("incendio-texto")
        elemento.style.color = "#000000"
        elemento.innerHTML = "Sem detecção"
        addCard("Incendio terminado")
    }

    function onMessage(event) {
        console.log(event.data);
        var myObj = JSON.parse(event.data);
        var keys = Object.keys(myObj);
        if (myObj["sinalPresenca"]=="1" && !alarme){
            alarme = true
            ativarAlarme()
        }
        if (myObj["leituraFogo"]=="1"&& !incendio){
            incendio = true
            ativarIncendio()
        }
        if (myObj["sinalPresenca"]=="0" && alarme){
            alarme = false
            desativarAlarme()
        }
        if (myObj["leituraFogo"]=="0" && incendio){
            incendio = false
            desativarIncendio()
        }
        if (myObj["ativacaoAlarme"]=="0"){
            const botaoAlarme = document.querySelector("#alarme-button");
            botaoAlarme.innerHTML = "DESLIGADO";
            botaoAlarme.style.backgroundColor = "#FD002E";
        }
        if (myObj["ativacaoAlarme"]=="1"){
            const botaoAlarme = document.querySelector("#alarme-button");
            botaoAlarme.innerHTML = "LIGADO";
            botaoAlarme.style.backgroundColor = "#000dfdf2";
        }
        if (myObj["ativacaoIncendio"]=="0"){
            const botaoIncendio = document.querySelector("#incendio-button");
            botaoIncedio.innerHTML = "DESLIGADO";
            botaoIncedio.style.backgroundColor = "#FD002E";
        }
        if (myObj["ativacaoIncendio"]=="1"){
            const botaoIncendio = document.querySelector("#incendio-button");
            botaoIncendio.innerHTML = "LIGADO";
            botaoIncendio.style.backgroundColor = "#000dfdf2";
        }
        if(myObj["FonteUsada"]=="1"){
            alternarBateriaPrimaria();
        }
        if(myObj["FonteUsada"]=="0"){
            alternarBateriaSecundaria();
        }
    }
</script>
</html>
)rawliteral";

void removeLineInfo(char *input) {
    char *lineStart = strstr(input, "#line");
    
    while (lineStart != NULL) {
        char *lineEnd = strchr(lineStart, '\n');
        
        if (lineEnd != NULL) {
            char *secondLineEnd = strchr(lineEnd + 1, '\n');
            if (secondLineEnd != NULL) {
                // Move tudo após a segunda quebra de linha para o início da string
                memmove(lineStart, secondLineEnd + 1, strlen(secondLineEnd + 1) + 1);
            } else {
                // Se não houver segundo '\n', apenas remova a linha '#line'
                lineStart[0] = '\0';
            }
        } else {
            // Se não houver quebra de linha, apenas remova a linha "#line"
            lineStart[0] = '\0';
        }
        
        lineStart = strstr(input, "#line");
    }
}

void configureIOPin(){
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_PRESENCA, INPUT);
  pinMode(PIN_FOGO, INPUT);
}

void acionaBuzzer(){
  tone(PIN_BUZZER, 1000);
  delay(300);
  tone(PIN_BUZZER, 500);
  delay(200);
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void getSensorPresenca(){
  int sinalPresenca;
  if (ativacaoAlarme){
    sinalPresenca = digitalRead(PIN_PRESENCA);
  }
  else{
    sinalPresenca = 0;
  }
  String valorFogo = ((const char*) (readings["leituraFogo"]));
  if(sinalPresenca == HIGH){
    acionaBuzzer();
  }
  //LOW: Nada detectado
  else if (valorFogo=="0"){
    // Desativa o buzzer
    noTone(PIN_BUZZER);
  }
  readings["sinalPresenca"] = String(sinalPresenca);
  readings["ativacaoAlarme"] = String(ativacaoAlarme);  
}

void getSensorFogo(){
  bool leituraFogo;
  if (ativacaoIncendio){
    leituraFogo = digitalRead(PIN_FOGO);
  }else{
    leituraFogo = LOW;
  }
  String valorPresenca = ((const char*) (readings["sinalPresenca"]));
  if(leituraFogo==HIGH){
    acionaBuzzer();
  }
  else if (valorPresenca=="0"){
    noTone(PIN_BUZZER);
  }
  readings["leituraFogo"] =  String(leituraFogo);
  readings["ativacaoIncendio"] = String(ativacaoIncendio);
}

void getFonteUsada(){
  bool leituraFonte;
  leituraFonte = digitalRead(PIN_FONTE);
  readings["FonteUsada"] =  String(leituraFonte);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    //Check if the message is "getReadings"
    if (strcmp((char*)data, "getReadings") == 0) {
      // if it is, send current sensor readings
      getSensorFogo();
      getSensorPresenca();
      String sensorReadings = JSON.stringify(readings);
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}


void configurarWifi(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wifi....");
  }
  Serial.print("IP para conectar: ");
  Serial.println(WiFi.localIP());
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void configurarRotas(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/desativaralarme", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Alarme desativado");
    ativacaoAlarme = false;
    request->send_P(200, "text/html", "Desativado com sucesso");
  });
  server.on("/ativaralarme", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Alarme ativado");
    ativacaoAlarme = true;
    request->send_P(200, "text/html", "Ativado com sucesso");
  });
  server.on("/desativarincendio", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Incendio desativado");
    ativacaoIncendio = false;
    request->send_P(200, "text/html", "Desativado com sucesso");
  });
  server.on("/ativarincendio", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Incendio ativado");
    ativacaoIncendio = true;
    request->send_P(200, "text/html", "Ativado com sucesso");
  });
}


void enviarDados(){
  String sensorReadings = JSON.stringify(readings);
  if (ultimoResultado == " "){
    Serial.println(sensorReadings);
    Serial.println("Primeira execucao");
    ultimoResultado = sensorReadings;
  }
  if (ultimoResultado != sensorReadings){
    ultimoResultado = sensorReadings;
    Serial.println(sensorReadings);
    notifyClients(sensorReadings);
  }
  lastTime = millis();
}

void taskGetSensorPresenca( void *pvParameters ){
  while (1){
    getSensorPresenca();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void taskGetSensorFogo( void *pvParameters ){
  while (1){
    getSensorFogo();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void taskGetFonteUsada( void *pvParameters ){
  while (1){
    getFonteUsada();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void criarTarefas(){
  xTaskCreatePinnedToCore(
    taskGetSensorPresenca
    ,  "Leitura presenca" // A name just for humans
    ,  1024  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL
    , 0); //Task Handle
    xTaskCreatePinnedToCore(
      taskGetSensorFogo
      ,  "Leitura fogo" // A name just for humans
      ,  1024  // Stack size
      ,  NULL //Parameters for the task
      ,  1  // Priority
      ,  NULL
      , 0); //Task Handle
    xTaskCreatePinnedToCore(
      taskGetFonteUsada
      ,  "Leitura fogo" // A name just for humans
      ,  1024  // Stack size
      ,  NULL //Parameters for the task
      ,  1  // Priority
      ,  NULL
      , 0); //Task Handle
}

void setup() {
  initWebSocket();
  removeLineInfo(index_html);
  // Corrigir linhas extras adicionadas ao html
  ledcSetup(1, 2, 3);
  // Para corrigir um bug
  configureIOPin();

  Serial.begin(9600);

  configurarWifi();

  configurarRotas();

  server.begin();

  criarTarefas();
}

void loop() {
  while (1){
    if ((millis() - lastTime) > timerDelay) {
      enviarDados();
    }
  }
}