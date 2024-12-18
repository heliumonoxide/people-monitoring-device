// eslint-disable-next-line no-unused-vars
import React, { useEffect, useState } from 'react';

const EthicCard = () => {
  const [highestSpeed, setHighestSpeed] = useState(null);
  const [timeAdded, setTimeAdded] = useState(null);
  const [error, setError] = useState('');

  useEffect(() => {
    const fetchHighestSpeed = async () => {
      try {
        // Extract sums and format timeAdded
        const response = await fetch('https://webapi-two-flax.vercel.app/api/ethic-highest'); // Assuming this is the endpoint
        if (!response.ok) throw new Error('Failed to fetch data');
        const data = await response.json();
        const speed = data.map(doc => {
            return doc.speed
        })
        
        const timeAdded = data.map(doc => {
            const utcDate = new Date(doc.timeAdded._seconds * 1000); // Create a date object
            const localDate = new Date(utcDate.setHours(utcDate.getHours())); // Adjust to UTC+7
            return localDate.toString(); // Convert back to ISO string for chart
          });
        setHighestSpeed(speed); // Assuming API response format is { count: <number> }
        setTimeAdded(timeAdded); // Assuming API response format is { count: <number> }
      } catch (err) {
        setError(err.message);
      }
    };

    fetchHighestSpeed();
    // console.log(timeAdded)
  }, []);

  return (
    <div className="flex items-center justify-center">
      <div className="max-w-sm w-full bg-white shadow-lg rounded-lg overflow-hidden">
        <div className="bg-purple-500 text-white py-4 px-6 text-center">
          <h2 className="text-xl font-bold">Highest Speed Limit Breaker</h2>
        </div>
        <div className="px-6 py-3 flex flex-col items-center">
          {error ? (
            <p className="text-red-500">{error}</p>
          ) : highestSpeed !== null ? (
            <>
              <p className="text-2xl font-extrabold text-purple-500">{highestSpeed}</p>
              <p className="text-lg font-semibold text-gray-700">km/hour</p>
            </>
          ) : (
            <p className="text-gray-500">Loading...</p>
          )}
          {error ? (
            <p className="text-red-500">{error}</p>
          ) : timeAdded !== null ? (
            <>
              <p className="text-center text-sm font-semibold text-gray-700">{timeAdded}</p>
            </>
          ) : (
            <p className="text-gray-500">Loading...</p>
          )}
        </div>
      </div>
    </div>
  );
};

export default EthicCard;
