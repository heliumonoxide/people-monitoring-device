// eslint-disable-next-line no-unused-vars
import React, { useEffect, useState } from 'react';

const Decision = () => {
    const [highestCount, setHighestCount] = useState(null);
    const [newestCount, setNewestCount] = useState(null);
    const [decision, setDecision] = useState(null);
    const [error, setError] = useState('');

    useEffect(() => {
        const fetchHighestCount = async () => {
            try {
                // Extract sums and format timeAdded
                const response = await fetch('/api/highest'); // Assuming this is the endpoint
                const response2 = await fetch('/api/newest-sum');
                if (!response.ok) throw new Error('Failed to fetch data');
                if (!response2.ok) throw new Error('Failed to fetch data');
                const data = await response.json();
                const data2 = await response2.json();
                const highSum = data.map(doc => {
                    return doc.sum
                })
                const newSum = data2.map(doc => {
                    return doc.sum
                })
                setHighestCount(highSum); // Assuming API response format is { count: <number> }
                setNewestCount(newSum); // Assuming API response format is { count: <number> }

                if (newSum / highSum > 0.7) {
                    setDecision("Crowded");
                }
                else if (newSum / highSum <= 0.7) {
                    setDecision("Not Crowded")
                }
            } catch (err) {
                setError(err.message);
            }
        };

        fetchHighestCount();
    }, []);

    return (
        <div className="flex items-center justify-center">
            <div className="max-w-sm w-full bg-white shadow-lg rounded-lg overflow-hidden">
                <div className="bg-purple-500 text-white py-4 px-6 text-center">
                    <h2 className="text-xl font-bold">Latest Condition</h2>
                </div>
                <div className="px-6 py-3 flex flex-col items-center">
                    {error ? (
                        <p className="text-red-500">{error}</p>
                    ) : decision !== null ? (
                        <>
                            <p className="text-2xl font-extrabold text-purple-500 ">{decision}</p>
                            {/* <p className="text-lg font-semibold text-gray-700">person</p> */}
                        </>
                    ) : (
                        <p className="text-gray-500">Loading...</p>
                    )}
                    <div className='flex flex-row'>
                    {error ? (
                        <p className="text-red-500">{error}</p>
                    ) : newestCount !== null ? (
                        <>
                            <p className="text-lg font-normal text-black ">{newestCount}</p>
                        </>
                    ) : (
                        <p className="text-gray-500">Loading...</p>
                    )} /
                    {error ? (
                        <p className="text-red-500">{error}</p>
                    ) : highestCount !== null ? (
                        <>
                            <p className="text-lg font-normal text-black ">{highestCount}</p>
                        </>
                    ) : (
                        <p className="text-gray-500">Loading...</p>
                    )}
                    </div>
                    
                </div>
            </div>
        </div>
    );
};

export default Decision;
