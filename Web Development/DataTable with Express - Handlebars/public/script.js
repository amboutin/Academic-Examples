var array;
var names = ["name", "reps", "weight", "date", "lbs"];
var eleCreate2;

document.addEventListener('DOMContentLoaded', getData.bind(this, array));


/******************************************************************************* 
**	These functions use AJAX POST and GET requests to change the client table and server table
*******************************************************************************/

function insert(){
	var stuff;
	var work = {};
	var att = document.getElementsByTagName("input");
	for(i = 0; i < att.length; i++){
		stuff = att[i].name;
		work[stuff] = att[i].value ;  		
	}
	var req = new XMLHttpRequest();
	req.open("Post", "http://flip3.engr.oregonstate.edu:64010/insert", true);
	req.setRequestHeader("Content-Type", "application/json");
	req.addEventListener('load', function(){
		var check = JSON.parse(req.responseText);
		if(check.affectedRows == 1){
			stuff = JSON.parse(req.responseText);
			var insertId = "insertId";
			var	eleCreate = document.createElement("tr");
			eleCreate.setAttribute("id", stuff[insertId]);
			var	eleHold = document.getElementsByTagName("tbody");
			eleHold[0].appendChild(eleCreate);
			for(j = 0; j < 5; j++){								// An inner loop provides the columns for each row created
				eleHold = document.createElement("td");
				eleCreate.appendChild(eleHold);
				var value = names[j];
				eleHold.textContent = work[value];			
				eleHold.style.backgroundColor = "rgb(22,160,190)";
			}
			eleHold = document.createElement("button");
			eleHold.attributes.type="button";
			eleHold.attributes.value="Delete";
			eleHold.textContent = "Delete";
			eleCreate.appendChild(eleHold);
			eleHold.addEventListener("click", deleteRow.bind(this,'table1', stuff[insertId]));

			eleHold = document.createElement("button");
			eleHold.attributes.type="button";
			eleHold.attributes.value="Edit";
			eleHold.textContent = "Edit";
			eleCreate.appendChild(eleHold);
			eleHold.addEventListener("click", editRow.bind(this,'table1', stuff[insertId]));
		}
	})
	req.send(JSON.stringify(work));	
}

function getData(array){
	var req = new XMLHttpRequest();
	req.open("GET", "http://flip3.engr.oregonstate.edu:64010/getData", true);
	req.addEventListener('load', function(){
		var response = JSON.parse(req.responseText);
		array = response;
		for(i = 0; i < array.length; i++){									// Here, a loop creates each other row
			var	eleCreate = document.createElement("tr");
			eleCreate.setAttribute("id", array[i].id);
			var	eleHold = document.getElementsByTagName("tbody");
			eleHold[0].appendChild(eleCreate);
				for(j = 0; j < 5; j++){								// An inner loop provides the columns for each row created
					eleCreate = document.createElement("td");
					eleHold = document.getElementsByTagName("tr");
					eleHold[i+1].appendChild(eleCreate);
					var value = names[j];
					eleCreate.textContent = array[i][value];			
					eleCreate.style.backgroundColor = "rgb(22,160,190)";
				}
			eleCreate = document.createElement("button");
			eleCreate.attributes.type="button";
			eleCreate.attributes.value="Delete";
			eleCreate.textContent = "Delete";
			eleHold = document.getElementsByTagName("tr");
			eleHold[i + 1].appendChild(eleCreate);
			eleCreate.addEventListener("click", deleteRow.bind(this,'table1', array[i].id));

			eleCreate = document.createElement("button");
			eleCreate.attributes.type="button";
			eleCreate.attributes.value="Edit";
			eleCreate.textContent = "Edit";
			eleHold = document.getElementsByTagName("tr");
			eleHold[i + 1].appendChild(eleCreate);
			eleCreate.addEventListener("click", editRow.bind(this,'table1', array[i].id,array));
		}
	})
req.send(null);
}

function deleteRow(tableID,currentRow) {		// this is modified code as found at http://jsfiddle.net/GRgMb/118/
    try {
        var table = document.getElementById(tableID);
        var rowCount = table.rows.length;
        for (var i = 0; i < rowCount; i++) {
            var row = table.rows[i];
            var thisOb = document.getElementById(currentRow);
            if (row==thisOb) {
				var req = new XMLHttpRequest();
				req.open("Get", "http://flip3.engr.oregonstate.edu:64010/delete?id=" + currentRow , true);
				req.addEventListener('load', function(){
					console.log(req.responseText);
					var check = JSON.parse(req.responseText);
					if(check.affectedRows == 1){
						table.deleteRow(i);
						rowCount--;
						i--;
					}
				})
				req.send(null);
			break;
            }
        }
    } catch (e) {
        alert(e);
    }
}

function editRow(tableID,currentRow,array){
	var thisOb = document.getElementById(currentRow);
	for(j = 0; j < 7; j++){								// A loop provides the columns for each row created
		thisOb.removeChild(thisOb.firstElementChild);
	}
	for(j = 0; j < 5; j++){								// A loop provides the columns for each row created
		eleCreate2 = document.createElement("td");
		eleCreate = document.createElement("input");
		eleCreate.attributes.type ="text";
		var value = names[j];
		eleCreate.setAttribute("name", value);
		eleCreate.attributes.size ="10";
		eleCreate.attributes.maxlength ="100";	
		eleCreate.style.width = "100px";
		eleCreate.style.textAlign = "center";	
		eleCreate2.appendChild(eleCreate);
		console.log(eleCreate);
		eleHold = thisOb;
		eleHold.appendChild(eleCreate2);
		var index = array.map(function(e) { return e.id; }).indexOf(currentRow);
		eleCreate.value = array[index][value];			
	}
	eleCreate = document.createElement("button");
	eleCreate.attributes.type="button";
	eleCreate.attributes.value="Save";
	eleCreate.textContent = "Save";
	eleCreate.addEventListener("click", updateRow.bind(this,'table1', currentRow,array));
	eleHold = thisOb;
	eleHold.appendChild(eleCreate);
}

function updateRow(tableID,currentRow,array){

	var thisOb = document.getElementById(currentRow);
	var stuff;
	var work = {};
	var att = thisOb.getElementsByTagName("input");
	for(i = 0; i < 5; i++){
		stuff = att[i].name;
		work[stuff] = att[i].value ;  		
	}
	work["id"] = currentRow;
	
	var req = new XMLHttpRequest();
	req.open("Post", "http://flip3.engr.oregonstate.edu:64010/safe-update", true);
	req.setRequestHeader("Content-Type", "application/json");
	req.addEventListener('load', function(){
		var check = JSON.parse(req.responseText);
		if(check.affectedRows == 1){
			stuff = JSON.parse(req.responseText);
			for(j = 0; j < 6; j++){								// A loop provides the columns for each row created
				thisOb.removeChild(thisOb.firstElementChild);
			}
			var eleCreate, eleHold;
			for(j = 0; j < 5; j++){								// A loop provides the columns for each row created
				eleCreate = document.createElement("td");
				eleHold = thisOb;
				eleHold.appendChild(eleCreate);
				var value = names[j];
				eleCreate.textContent = work[value];			
				eleCreate.style.backgroundColor = "rgb(22,160,190)";
			}
			eleCreate = document.createElement("button");
			eleCreate.attributes.type="button";
			eleCreate.attributes.value="Delete";
			eleCreate.textContent = "Delete";
			eleHold = thisOb;
			eleHold.appendChild(eleCreate);
			eleCreate.addEventListener("click", deleteRow.bind(this,'table1', currentRow));
			
			eleCreate = document.createElement("button");
			eleCreate.attributes.type="button";
			eleCreate.attributes.value="Edit";
			eleCreate.textContent = "Edit";
			eleHold = thisOb;
			eleHold.appendChild(eleCreate);
			eleCreate.addEventListener("click", editRow.bind(this,'table1', currentRow,array));
		}
	})
	req.send(JSON.stringify(work));	
}


