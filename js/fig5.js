  google.charts.load('current', {'packages':['bar']});
  google.charts.setOnLoadCallback(drawChart);

  function drawChart() {
    var data = google.visualization.arrayToDataTable([
    ['Average P(t|C)', 'Inconsistent', 'Consistent'],
    ['Implementation', 4.4 , 10.3 ],
    ['Interface (input)', 3 , 4.5 ],
    ['Interface (output)', 3.1 , 4.9 ],
    ['Enclosing', 1.5 , 2.7 ]
    ]);

    var options = {
    	title: 'The average conditional occurrence of tokens in method names on the contexts'
  	 };

    var chart = new google.charts.Bar(document.getElementById('fig5'));

    chart.draw(data, google.charts.Bar.convertOptions(options));
  }