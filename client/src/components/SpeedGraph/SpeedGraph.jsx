/* eslint-disable no-unused-vars */
// src/components/LineChart.jsx
import React, { useEffect, useState } from 'react';
import Chart from 'react-apexcharts';

const SpeedGraph = () => {
  const [chartData, setChartData] = useState({
    series: [{ name: 'Kecepatan', data: [] }],
    options: {
      colors: ['#581c87'],
      chart: {
        height: 200,
        type: 'line',
        zoom: { enabled: true},
        offsetY: -20,
      },
      dataLabels: { enabled: false },
      stroke: {
        curve: 'smooth',
        colors: '#581c87'
      },
      markers: {
        colors: "#581c87"
      },
      xaxis: {
        categories: [], // This will hold the timeAdded values
        type: 'datetime', // Set the type to datetime
      },
      title: {
        text: 'Kecepatan terbaru di Halte FTUI',
        align: 'left',
        style: {
          fontSize: '14px',
          fontWeight: 'bold',
          color: '#581c87'
        },
      },
      grid: {
        row: {
          colors: ['#f3f3f3', 'transparent'], // Takes an array which will be repeated on columns
          opacity: 0.5,
        },
      },
      tooltip: {
        enabled: true,
        shared: true, // Allows displaying multiple series data
        intersect: false, // Show tooltip even if not hovering directly over a point
        x: {
          show: true,
          format: 'dd MMM yyyy HH:mm:ss',
        },
        y: {
          show: true,
          formatter: function (val) {
            return `${val} km/h`
          }
        },
        markers: {
          show: true,
          colors: '#581c87'
        }
      },
    },
  });

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('/api/ethic-count');
        const data = await response.json();

        // Ensure we have an array
        const results = Array.isArray(data) ? data : [];

        // Extract sums and format timeAdded
        const speeds = results.map(doc => doc.speed); // Extract the sum field
        const timeAdded = results.map(doc => {
          const utcDate = new Date(doc.timeAdded._seconds * 1000); // Create a date object
          const localDate = new Date(utcDate.setHours(utcDate.getHours() + 7)); // Adjust to UTC+7
          return localDate.toISOString(); // Convert back to ISO string for chart
        });

        // Update chart data
        setChartData(prevState => ({
          ...prevState,
          series: [{ ...prevState.series[0], data: speeds }],
          options: {
            ...prevState.options,
            xaxis: { categories: timeAdded }, // Set the x-axis categories
          },
        }));
      } catch (error) {
        console.error('Error fetching data:', error);
      }
    };

    fetchData();
  }, []);

  return (
    <div className="chart-container">
      <Chart
        options={chartData.options}
        series={chartData.series}
        type="line"
        height={200}
      />
    </div>
  );
};

export default SpeedGraph;
