# Yuri Operating System

オープンソースで開発を行っている32bitオペレーティングシステムです。

An open source 32bit operating systems!

ゆりOSはCとアセンブリ(NASM)で開発しています。

## ゆりOSの紹介
- できること
  - なんかHDDとかの補助記憶装置にアクセスできる(現在yurifsの開発を行っています)
  - なんかしょぼいシェルとしょぼいコマンド群がある
  - なんかCUIベースでいろいろできる(しょぼいですが)
  - なんか窓際の埃のようなしょぼいGUIがある
  - なんかシェルがスクロールする
- ライセンスについて(下にも書いてあります)
  - 私としてはオープンソースというものは感染すべきではないと思っております。様々な人たちが自由にソースコードを引用できるべきなのです。なので緩いMITライセンスにしました。
  - 私が開発したゆりOSのデフォルトフォントも、もちろんMITライセンスが適用されております。
- 開発のきっかけ
  - もともとOSに興味があり、川合秀実著　「30日でできる! OS自作入門」に感化され開発をはじめました。

### スクリーンショット

#### CUIモード
![Imgur](http://i.imgur.com/02xXbaP.png)

#### GUIモード
![Imgur](http://i.imgur.com/83mc2xg.png)

##Building

Yuriのディレクトリに入って以下のコマンドを打ってください。


```
make all
```

これでディスクイメージが出来上がります。

環境にもよりますが数秒でビルドは完了します。


###ビルドに必要なもの


・GCC（32bit）


・NASM

開発者側のバージョン

・GCC 6.2.0

・NASM 2.10.09

###ビルド環境の構築例(openSUSE Leap 42.2の場合)

```
$ sudo zypper in make gcc gcc-32bit nasm
```

## Contributing

バグの報告やYuri開発に協力したい人、感想などがあればat.sisy@gmail.comまでメールを送っていただければ幸いです。もちろんプルリクエストなどもウェルカムです。

##Lisence
Yuriは[MIT Lisence](https://opensource.org/licenses/MIT)を採用しています。
