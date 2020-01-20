
	 
	 
    google.charts.load('current', {'packages':['bar']});
    google.charts.setOnLoadCallback(drawBasic2);

    function drawBasic2() {
      var data = google.visualization.arrayToDataTable([
      ['', '% methods'],
        ['At least 33.3%', 82.50],
        ['At least 50%', 77.60],
        ['At least 66.7%', 58.10],
        ['100%', 35.60],
      ]);

      var options = {
      	title: 'The percentages of the methods whose names share certain tokens with the method contexts'
    	 };

      var chart = new google.charts.Bar(document.getElementById('fig42'));

      chart.draw(data, google.charts.Bar.convertOptions(options));
    }