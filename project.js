
function selectPhases(projectSelector, phaseSelectorId) {

      // Issue an asynchronous request to the server for a list of all phases
      // json data will be returned in the form:
      // [ { ID: "11", Name: "phase1", Description: "my test phase", ProjectID: "1", Logdirectory: "/tmp/log876"} ]
      dojo.xhrGet({
		url: "rest/phase/",
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
	

