google.charts.load('current', {'packages':['table']});
      google.charts.setOnLoadCallback(drawTable);

      function drawTable() {
        var data = new google.visualization.DataTable();
        data.addColumn('string', '');
        data.addColumn('string', 'Method name');
		  data.addColumn('string', 'Token');
        data.addRows([
          ['Mean #occcurrence',  '4.8', '400.3'],
          ['Median #occcurrence',   '1',  '3'],
          ['#occcurrence = 1', '62.8%', '23.3%'],
          ['#occcurrence > 1',   '37.2%',  '76.7%']
        ]);

        var table = new google.visualization.Table(document.getElementById('table1'));

        table.draw(data, {width: '100%', height: '100%'});
      }