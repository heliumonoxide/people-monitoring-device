// src/components/Dashboard.jsx
// eslint-disable-next-line no-unused-vars
import React from 'react';
import LineChart from '../PeopleGraph/PeopleGraph';
import ImageViewer from '../NewestImageComponent/NewestImageComponent';
import HighestCount from '../HighestCount/HighestCount';
import Decision from '../Decision/Decision';
import EthicCard from '../EthicCard/EthicCard';
import SpeedGraph from '../SpeedGraph/SpeedGraph';

const Dashboard = () => {
  return (
    <div className="min-h-screen bg-gradient-to-b from-purple-300 to-white text-gray-800 flex flex-col">
      <header className="bg-purple-500 p-4 shadow-md text-white">
        <h1 className="text-2xl font-bold">People Monitoring in Halte FTUI</h1>
      </header>

      <main className="flex flex-col p-6">
        <div className="flex flex-wrap mx-10 gap-5 flex-row justify-center">
          <ImageViewer />
          <div className='flex flex-col gap-5'>
            <HighestCount />
            <Decision />
          </div>
          <EthicCard />
        </div>
        <div className="container mx-auto mt-10">
          <h2 className="text-violet-900 text-xl mb-4 mx-6">Grafik-Grafik Monitoring</h2>
          <div className="flex flex-col bg-white gap-10 rounded-lg shadow-lg p-6 pt-12 mx-6">
            <LineChart />
            <SpeedGraph />
          </div>
        </div>
      </main>

      <footer className="bg-purple-500 text-white p-4 text-center">
        <p>&copy; {new Date().getFullYear()} Universitas Indonesia</p>
      </footer>
    </div>
  );
};

export default Dashboard;
