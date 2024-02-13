let loc = window.location.host.split(":");
let socket = new WebSocket("ws://" + loc[0] + ":81");
socket.binaryType = "arraybuffer";

socket.onopen = function(e) {
  console.log("[open] Connection established");
  console.log("Sending to server");
};

socket.onmessage = function(event) {
  if(event.data instanceof ArrayBuffer) {
    // binary frame
    VICRAM = new Uint8Array(event.data);

    refreshDisplay();
  } else {
    // text frame
    console.log(event.data);
  }
};
let totalTime=0;
socket.onclose = function(event) {
  if (event.wasClean) {
    console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
  } else {
    // e.g. server process killed or network down
    // event.code is usually 1006 in this case
    console.log('[close] Connection died');
  }
};

socket.onerror = function(error) {
  console.log(`[error] ${error.message}`);
};

let VICRAM;
let RAM_SIZE=1025;

function printLog(msg){let log=document.getElementById('Log');log.innerHTML=log.innerHTML+msg+'<br>';log.scrollTop = log.scrollHeight}
document.addEventListener('DOMContentLoaded',initCanvas,false);
let fC=0;
let cS=0;
// setInterval(updateFPS,1000);
setInterval(function(){cS=!cS;},500);
// function updateFPS(){
//   let fT = document.getElementById('fps');
//   fT.innerHTML=fC+ ' fps';
//   fC=0;
// }
let pal=['#000000','#FFFFFF','#880000','#AAFFEE',
  '#CC44CC','#00CC55','#0000AA','#EEEE77',
  '#DD8855','#664400','#FF7777','#333333',
  '#777777','#AAFF66','#0088FF','#BBBBBB'];

function initCanvas(){
  printLog('initCanvas');

  VICRAM = new Uint8Array(RAM_SIZE);
  document.addEventListener('keypress', keypress, false);
}

const sendLimit = 1 ;

function split(str) {
  return [str.slice(0, sendLimit), str.slice(sendLimit)];
}
const DEF_DELAY = 100;
function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms || DEF_DELAY));
}
let clipboardPending;
document.addEventListener('paste', (event) => {
  if(clipboardPending) {
    alert("Paste already in progress please wait...");
    return;
  }
  clipboardPending = true;
  paste(event.clipboardData.getData('text/plain')).then(function () {
    clipboardPending = false;
    // return;
  });

});


async function paste(mystr) {
  let res;
  let ctr = 0;
  while (1) {
    ctr++;
    res = split(mystr);

    socket.send(res[0]);
    await sleep();

    if(res[1].length) {
      mystr = res[1];
    } else {
      return;
    }
  }

}

function refreshDisplay(){
  let petscii;
  let myCharr = '';
  let line = 0;
  for( let i=0; i< VICRAM.length-24; i++) {
    petscii=VICRAM[24+i];//char rom starts at 24 offset
    if (petscii<32) petscii = petscii + 64;
    if(i!==0 && (i%40)===0) {
      myCharr += '\n';
      line++;
    }
    if (VICRAM[0] === line &&  VICRAM[1] === (i%40)) {
      myCharr += String.fromCharCode(9608);
    } else {
      myCharr += String.fromCharCode(petscii);
    }
  }
  document.getElementById('content').innerText = myCharr;
  // return;

  // if (cS) ctx.fillRect(32+VICRAM[1]*8,32+VICRAM[0]*8,8,8);
  // fC++;
}

function keypress(e) {
  let keyCode = e.keyCode;
  socket.send(String.fromCharCode(keyCode));
}
