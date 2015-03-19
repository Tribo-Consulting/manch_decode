/***/

var interval = 32;

var buffer = {
	b : [], //buffer array
	p : 0   //position
}

var done = true;

  //                        sync
  //                            \ checksum
  //                          ....||||      y           o            u 
//var inputData = (""+"11110100"+"01111001"+"01101111"+"01110101").split('');

var preamble = "11110100";

//Convert bit sequence into a manchester bit sequence
function manchesterEncode(binaryArray){
	
	var manchesterSequence = [];
	for(var i=0; i<binaryArray.length; i++){

		if(binaryArray[i]=="1"){ //1
			manchesterSequence.push(0);
			manchesterSequence.push(1);
		}
		else{ //0
			manchesterSequence.push(1);
			manchesterSequence.push(0);
		}
	}

	return manchesterSequence;
}


function send(binaryString){

	binaryString = preamble+binaryString; //prepend preamble string;

	var binaryArray = binaryString.split(''); //convert binary string into binary array
	var manchesterArray = manchesterEncode(binaryArray); //convert binary array in manchester array

	buffer.p = 0;
	buffer.b = manchesterArray;

	done = false;
}

function stop(){
	done = true;
	buffer.p = 0;
	buffer.b = [];
}

function digitalWrite(bool){
	flickDiv.style.backgroundColor = bool?"#FFFFFF":"#000000";
}


/***********/


function sendButtonAction(){

	interval = speed.value;

	var text = data.value;
	var binaryString = textToBin(text);

	send(binaryString);

	logStatus("Started");
	logData(binaryString);
	logRaw(buffer.b.join(""));
}

function stopButtonAction(){
	logStatus("Stopped!");

	stop();
}

function textToBin(string){

	var PADDING = "00000000";

	var resultArray = []

	for (var i = 0; i < string.length; i++) {
	  var compact = string.charCodeAt(i).toString(2)
	  var padded  = compact.substring(0, PADDING.length - compact.length) + compact

	  resultArray.push(padded)
	}
	
	return resultArray.join('');
}