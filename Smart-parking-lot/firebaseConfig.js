import { initializeApp } from 'firebase/app';
import { getDatabase } from 'firebase/database';

const firebaseConfig = {
  apiKey: 'AIzaSyCCTjKWELtwx1XzAe0dM_1F-E-wlXYR_Lk',
  authDomain: 'ledrgb-aba5c.firebaseapp.com',
  databaseURL: 'https://ledrgb-aba5c-default-rtdb.firebaseio.com',
  projectId: 'ledrgb-aba5c',
  storageBucket: 'ledrgb-aba5c.appspot.com',
  messagingSenderId: '361134493701',
  appId: '1:361134493701:web:f3f1c03128eabd9ccd4c6c',
};

// Initialize Firebase
export const app = initializeApp(firebaseConfig);
export const database = getDatabase(app);
