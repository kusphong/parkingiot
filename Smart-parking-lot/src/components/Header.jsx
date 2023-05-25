import React from 'react';
import logo from '../assets/logo.png';
import '../styles/Header.css';

const Header = () => {
  return (
    <div className='header'>
      <img className='logo' src={logo} />
      <div className='header-content'>
        <h1 className='header-title'>Phát triển hệ thống IoT</h1>
        <h1 className='header-title'>BÃI ĐỖ XE TỰ ĐỘNG</h1>
      </div>
      <div className='empty' />
    </div>
  );
};

export default Header;
