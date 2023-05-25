import React from 'react';
import '../styles/ParkingLot.css';
import car from '../assets/car.png';

const ParkingLot = ({ isCar, number }) => {
  return (
    <div className={isCar ? 'container iscar' : 'container'}>
      {isCar ? <img className='car' src={car} /> : <h1 className='number'>{number}</h1>}
    </div>
  );
};

export default ParkingLot;
