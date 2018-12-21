#include "APP.h"



void APP_Init(void)
{

}




using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Globalization;
namespace HextoBin
{
    public partial class Form1 : Form
    {
        OpenFileDialog file = new OpenFileDialog();
        SaveFileDialog Savefile = new SaveFileDialog();
        FolderBrowserDialog dialog = new FolderBrowserDialog();
        private string foldPath;
        private string szHex;
       // private string Name;
        //private string Path;
 
        public Form1()
        {
            InitializeComponent();
        }
 
        private void button1_Click(object sender, EventArgs e)
        {
            file.Filter = "�������ļ�(*.hex)|*.hex";//�ļ�������
            //�����ϴδ򿪵�Ŀ¼
            file.RestoreDirectory = true;
            //�����Ƿ������ѡ
            file.Multiselect = false;
            if (file.ShowDialog() == DialogResult.OK)
            {
                //��ʾ�ļ�������չ��
               // textBox1.Text = System.IO.Path.GetFileName(file.SafeFileName);
                textBox1.Text = file.FileName;
            }
        }
        private void button2_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "")
            {
                StreamReader HexReader = new StreamReader(file.FileName);         
                try
                {
                    while (true)
                    {
                        string szLine = HexReader.ReadLine(); //��ȡһ������
                        if (szLine == null) //����������
                        {
                            break;
                        }
                        if (szLine.Substring(0, 1) == ":") //�жϵ�1�ַ��Ƿ���:
                        {
 
                            if (szLine.Substring(1, 8) == "00000001")//���ݽ���
                            {
                                
                                break;
                            }
                            if ((szLine.Substring(1, 1) == "1") ||(szLine.Substring(3,4)!="0000"&&szLine.Substring(1, 1) == "0"))
/**/
                            {
                                szHex += szLine.Substring(9, szLine.Length - 11); //��ȡ��Ч�ַ�����0��1
                            }
                        }
                    }
                }
                catch
                { }
                Int32 i;
                Int32 j = 0;
                Int32 Length = szHex.Length;
                byte[] szBin = new byte[Length / 2];
                for (i = 0; i < Length; i += 2) //���ַ��ϲ���һ��16�����ֽ�
                {
                    szBin[j] = (byte)Int16.Parse(szHex.Substring(i, 2), NumberStyles.HexNumber);
                    j++;
 
                }
                if (textBox2.Text != "")
                {
                    try
                    {
                        FileStream fs = new FileStream(Savefile.FileName, FileMode.Create);
                        //FileStream fs =File.Create(foldPath);                      
                        BinaryWriter bw = new BinaryWriter(fs);
                        for (int k = 0; k < j; k++)
                        {
                            bw.Write(szBin[k]);                          
                        }                      
                        bw.Close();
                        fs.Close();
                        MessageBox.Show("ת�����","��ʾ");
                    }
                    catch
                    { }
 
                }
                else
                {
                    MessageBox.Show("��ѡ��bin�ļ�����·��","��ʾ");
                }
 
 
            }
            else
            {
                MessageBox.Show("��ѡ��һ��Hex�ļ�", "��ʾ");
            }
          
            
 
        }
 
        private void button3_Click(object sender, EventArgs e)
        {
 
           
            Savefile.RestoreDirectory = true;
            Savefile.Filter = "Bin�ļ�(*.bin)|*.bin";
            if (Savefile.ShowDialog() == DialogResult.OK && Savefile.FileName.Length > 0)
            {
                textBox2.Text = Savefile.FileName;
                //textBox2.Text= Savefile.FileName.Substring(Savefile.FileName.LastIndexOf("\\") + 1);//�ļ���+��׺
               // textBox2.Text = Savefile.FileName.Substring(0,Savefile.FileName.LastIndexOf("\\"));//·��
            }
 
 
            
          /*  if (dialog.ShowDialog() == DialogResult.OK)
            {
                 foldPath = dialog.SelectedPath;             
                DirectoryInfo theFolder = new DirectoryInfo(foldPath);
                FileInfo[] dirInfo = theFolder.GetFiles();
                //�����ļ���
                foreach (FileInfo file in dirInfo)
                {
                    MessageBox.Show(file.ToString());
                }
                textBox2.Text = foldPath;
                       
            }*/
            
 
        }
 
        
    }
}










