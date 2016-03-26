
function selectPhases(projectSelector, phaseSelectorId) {

      // Issue an asynchronous request to the server for a list of all phases
      // json data will be returned in the form:
      // [ { ID: "11", Name: "phase1", Description: "my test phase", ProjectID: "1", Logdirectory: "/tmp/log876"} ]
      dojo.xhrGet({
		url: "rest/daf/phase/",
		load: function(data, ioargs) {
		         var selectedProjectID = projectSelector.options[projectSelector.selectedIndex].value;
		         var sel = '<select id="backupPhaseId" name="phase">';
		         for (var i=0; i<data.length; i++) {
		            if (data[i]["ProjectID"] == selectedProjectID) {
		               sel += '<option value="' + data[i]["Name"] + '">' + data[i]["Name"] + '</option>';
		            }   
		         }
		         sel += '</select>';
		         document.getElementById(phaseSelectorId).innerHTML = sel;	            
		      },
		error: function(data, ioargs) {
		         alert("An error occurred while invoking selectPhases()");     /* <<<<< need a better error handler */
		      },
		handleAs: 'json'
	  });	  

}

function selectPhases1(projectSelector, phaseselectortdID) {

      // Issue an asynchronous request to the server for a list of all  phases
      // json data will be returned in the form:
      // [ { ID: "11", Name: "phase1", Description: "my test phase", ProjectID: "1", Logdirectory: "/tmp/log876"} ]
      dojo.xhrGet({
		url: "rest/daf/phase/",
		load: function(data, ioargs) {
		         var selectedProjectID = projectSelector.options[projectSelector.selectedIndex].value;
		         //var sel = '<select id="backupPhaseId" name="phase">';
		         //for (var i=0; i<data.length; i++) {
		          //  if (data[i]["ProjectID"] == selectedProjectID) {
		           //    sel += '<option value="' + data[i]["Name"] + '">' + data[i]["Name"] + '</option>';
		          //  }   
		       //  }
		        // sel += '</select>';
		         //document.getElementById(phaseSelectorId).innerHTML = sel;
		         
		         dojo.empty(phaseselectortdID);
		         var sel = '';
		         var sid = dojo.byId(phaseselectortdID);
		         dojo.create("select", {innerHTML: "", id: "phaseselectorId", name: "phase"}, sid);
		         for (var i=0; i<data.length; i++) {
		            if (data[i]["ProjectID"] == selectedProjectID) {
		               dojo.create("option", {innerHTML: data[i]["Name"], value: data[i]["Name"]}, "phaseselectorId");
		            }
		         }
	            
		      },
		error: function(data, ioargs) {
		         alert("An error occurred while invoking selectPhases1()");     /* <<<<< need a better error handler */
		      },
		handleAs: 'json'
	  });	  

}

	

