import React from 'react';
import '../styles/Footer.css';

const Footer = () => {
  return (
    <div class='footer'>
      <div class='waves'>
        <div class='wave' id='wave1'></div>
        <div class='wave' id='wave2'></div>
        <div class='wave' id='wave3'></div>
        <div class='wave' id='wave4'></div>
      </div>
      <div class='footer-content'>
        <p> Khoa Điện tử - Viễn thông </p>
        <p> Học viện Kĩ thuật mật mã © 2023</p>
      </div>
    </div>
  );
};

export default Footer;
