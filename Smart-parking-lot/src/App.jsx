import { useEffect, useState } from 'react';
import { ref, onValue, set } from 'firebase/database';
import { database } from '../firebaseConfig';
import Header from './components/Header';
import ParkingLot from './components/ParkingLot';
import Footer from './components/Footer';

function App() {
  const [data, setData] = useState([]);
  const [total, setTotal] = useState(0);
  const [empty, setEmpty] = useState(0);

  useEffect(() => {
    const unsubscribe = onValue(ref(database, '/SmartParkingLot'), (snapshot) => {
      const response = snapshot.val();
      setData(response.split('').map((i) => i === '1'));
    });
    return () => {
      unsubscribe();
    };
  }, []);

  useEffect(() => {
    setEmpty(data.filter((i) => i === false).length);
    setTotal(data.length);
  }, [data]);

  useEffect(() => {
    set(ref(database, 'allPakingLotNumber/'), total);
    set(ref(database, 'emptyPakingLotNumber/'), empty);
  }, [total, empty]);

  return (
    <div className='App'>
      <Header />
      {data.filter((i) => i === false).length === 0 ? (
        <h1 className='info'> Đã hết chỗ trống, vui lòng đợi xe ra khỏi bãi đỗ</h1>
      ) : (
        <h1 className='info'>{`Chỗ trống: ${empty} / ${total}`}</h1>
      )}
      <div className='parking-lots-container'>
        {data.map((item, index) => (
          <ParkingLot key={index} isCar={item} number={index + 1} />
        ))}
      </div>
      <Footer />
    </div>
  );
}

export default App;
