# DOS multiple process
# รายละเอียด
    - วัตถุประสงค์เพื่อเสริมความเข้าใจเกี่ยวกับกลไกลการทำงานของการทำระบบ multiprocess ของระบบปฎิบัติการ
    - โปรแกรมรองรับเฉพาะระบบปฎิบัติการ DOS 16 bit และ DOXBOX
    - โปรแกรมรองรับเฉพาะการโหลดแฟ้ม ".COM" เท่านั้น
    - พัฒนาโดยใช้  compiler ของ Borland C (3.x)   

#แนวคิดการจัดการ process เป็นแบบ two-state-model
    https://www.geeksforgeeks.org/states-of-a-process-in-operating-systems/

# แฟ้ม
      1. MPROC.C  : source file ของโปรแกรม mulitple process
             การ compile
             bcc -mt mproc.c    (ได้ MPROC.exe)
             
      2. app3.c : source file ตัวอย่างที่ MPROC.exe โหลดมารัน 
             การ compile
             bcc -mt -lt app3.c (app3.com)

      3. app4.c : source file ตัวอย่างที่ MPROC.exe โหลดมารัน 
             การ compile
            bcc -mt -lt app4.c (app3.com)
             
  
      4. BCODE.ZIP : zip file ที่มีการ compile สำเร็จแล้ว
          การสั่ง run :   
          
 # การใช้งาน MPROC.EXE
     MPROC.EXE   สั่งโปรแกรม run

# สาธิตการ run

https://github.com/user-attachments/assets/e3f4a0c0-a253-4825-9135-4ed46f0105d2

     
# การสร้าง enviroment ในการสร้รงโปรแกรม
    สามารถดูได้จาก
      https://github.com/rapirakgmail/16bitWorld-bcc-env/edit/main/README.md


