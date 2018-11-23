<?xml version="1.0" ?><!DOCTYPE TS><TS language="fi" version="2.1">
<context>
    <name>CloneJob</name>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="146"/>
        <source>Writing data to %1 failed, %2 byte data should be written, but actually %3 wrote, error: %4</source>
        <translation>Tietojen kirjoittaminen kohteeseen %1 epäonnistui: vain %2 tavua tietoja kirjoitettiin, kun olisi pitänyt kirjoittaa %3 tavua, virhe: %4</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="193"/>
        <source>%1 not exist</source>
        <translation>Kohdetta %1 ei ole olemassa</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="207"/>
        <location filename="../src/corelib/clonejob.cpp" line="235"/>
        <source>%1 invalid or not exist</source>
        <translation>Kohde %1 on kelvoton tai sitä ei ole olemassa</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="219"/>
        <source>Disk only can be cloned to disk</source>
        <translation>Levy voidaan kloonata vain levylle</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="241"/>
        <source>%1 total capacity is less than maximum readable data on %2</source>
        <translation>Kohteen %1 kokonaistila on pienempi kuin luettavan tiedon kokonaismäärä kohteessa %2</translation>
    </message>
    <message>
        <location filename="../src/corelib/clonejob.cpp" line="255"/>
        <source>Failed to change %1 size, please check the free space on target disk</source>
        <translation>Kohteen %1 kokoa ei voitu muuttaa, ole hyvä ja tarkista kohdelevyllä oleva vapaa tila</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="220"/>
        <location filename="../src/widgets/mainwindow.cpp" line="592"/>
        <source>Cancel</source>
        <translation>Peruuta</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="326"/>
        <source>Select Operation</source>
        <translation>Valitse Toiminto</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="327"/>
        <source>Next</source>
        <translation>Seuraava</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="351"/>
        <location filename="../src/widgets/mainwindow.cpp" line="816"/>
        <source>Backup</source>
        <translation>Varmuuskopio</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="353"/>
        <location filename="../src/widgets/mainwindow.cpp" line="818"/>
        <source>Clone</source>
        <translation>Kloonaa</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="356"/>
        <location filename="../src/widgets/mainwindow.cpp" line="363"/>
        <source>Target disk will be permanently overwritten, please confirm to continue</source>
        <translation>Kohdelevy ylikirjoitetaan lopullisesti, ole hyvä ja vahvista jatkaaksesi</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="358"/>
        <location filename="../src/widgets/mainwindow.cpp" line="365"/>
        <source>Target partition will be permanently overwritten, please confirm to continue</source>
        <translation>Kohdeosio ylikirjoitetaan lopullisesti, ole hyvä ja vahvista jatkaaksesi</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="360"/>
        <location filename="../src/widgets/mainwindow.cpp" line="820"/>
        <source>Restore</source>
        <translation>Palauta</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="377"/>
        <source>Please move image file to other location outside the disk to avoid data loss</source>
        <translation>Ole hyvä ja siirrä kuvatiedosto toiseen sijaintiin levyn ulkopuolelle välttääksesi tietojen häviämisen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="379"/>
        <source>Please move image file to other location outside the partition to avoid data loss</source>
        <translation>Ole hyvä ja siirrä kuvatiedosto toiseen sijaintiin osion ulkopuolelle välttääksesi tietojen häviämisen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="392"/>
        <source>Storage location can not be in the disk to backup, please reselect</source>
        <translation>Tietojen sijainti ei voi olla varmuuskopioitavalla levyllä, ole hyvä ja valitse uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="394"/>
        <source>Storage location can not be in the partition to backup, please reselect</source>
        <translation>Tietojen sijainti ei voi olla varmuuskopioitavalla osiolla, ole hyvä ja valitse uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="413"/>
        <source>Not enough total capacity in target disk, please select another one</source>
        <translation>Kohdelevyllä ei ole tarpeeksi kokonaistilaa, ole hyvä ja valitse toinen levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="415"/>
        <source>Not enough total capacity in target partition, please select another one</source>
        <translation>Kohdeosiossa ei ole tarpeeksi kokonaistilaa, ole hyvä ja valitse toinen osio</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="440"/>
        <source>Not enough total capacity, please select another disk</source>
        <translation>Ei tarpeeksi kokonaistilaa, ole hyvä ja valitse toinen levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="469"/>
        <source>Proceed to clone?</source>
        <translation>Aloitetaanko kloonaaminen?</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="470"/>
        <source>All data in target loacation will be formated during cloning or restoring disk (partition) without cancelable operation.</source>
        <translation>Kaikki kohdesijainnin tiedot alustetaan levyn (osio) kloonauksen tai palautuksen alkaessa, joka ei ole peruutettavissa.</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="472"/>
        <source>Warning</source>
        <translation>Varoitus</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="473"/>
        <location filename="../src/widgets/mainwindow.cpp" line="524"/>
        <location filename="../src/widgets/mainwindow.cpp" line="642"/>
        <location filename="../src/widgets/mainwindow.cpp" line="767"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="489"/>
        <location filename="../src/widgets/mainwindow.cpp" line="570"/>
        <source>The selected storage location not found</source>
        <translation>Valittua tietojen sijaintia ei löydetty</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="715"/>
        <source>Failed to restart system</source>
        <translation>Järjestelmän uudelleenkäynnistäminen epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="749"/>
        <source>Failed to restart &quot;Deepin Recovery&quot;</source>
        <translation>&quot;Deepinin Palauttajaan&quot; uudelleenkäynnistäminen epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="519"/>
        <source>Restart to Continue</source>
        <translation>Käynnistä Uudelleen Jatkaaksesi</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="216"/>
        <source>Restore boot</source>
        <translation>Palauta käynnistys</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="503"/>
        <location filename="../src/widgets/mainwindow.cpp" line="558"/>
        <source>%1 not exist</source>
        <translation>Kohdetta %1 ei ole olemassa</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="522"/>
        <source>&quot;%1&quot; is used, please restart and enter &quot;Deepin Recovery&quot; to continue</source>
        <translation>&quot;%1&quot; on käytössä, ole hyvä, käynnistä uudelleen ja siirry &quot;Deepinin Palauttajaan&quot; jatkaaksesi</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="527"/>
        <source>&quot;%1&quot; is used, please install &quot;Deepin Recovery&quot; to retry</source>
        <translation>&quot;%1&quot; on käytössä, ole hyvä ja asenna &quot;Deepinin Palauttaja&quot; yrittääksesi uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="586"/>
        <source>Performing Backup</source>
        <translation>Varmuuskopioidaan</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="588"/>
        <source>Cloning</source>
        <translation>Kloonataan</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="590"/>
        <source>Restoring</source>
        <translation>Palautetaan</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="606"/>
        <source>Backup Failed</source>
        <translation>Varmuuskopionti Epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="612"/>
        <source>Clone Failed</source>
        <translation>Kloonaus Epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="614"/>
        <source>Restore Failed</source>
        <translation>Palautus Epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="619"/>
        <source>Retry</source>
        <translation>Yritä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="629"/>
        <source>Task done</source>
        <translation>Tehtävä suoritettu</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="632"/>
        <source>Backup Succeeded</source>
        <translation>Varmuuskopiointi Onnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="633"/>
        <source>View Backup File</source>
        <translation>Näytä Varmuuskopiotiedosto</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="641"/>
        <source>Clone Succeeded</source>
        <translation>Kloonaus Onnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="645"/>
        <source>Restore Succeeded</source>
        <translation>Palautus Onnistui</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="646"/>
        <source>Restart</source>
        <translation>Käynnistä Uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="774"/>
        <location filename="../src/widgets/mainwindow.cpp" line="792"/>
        <source>Loading</source>
        <translation>Ladataan</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="832"/>
        <source>Disk</source>
        <translation>Levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/mainwindow.cpp" line="834"/>
        <source>Partition</source>
        <translation>Osio</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="247"/>
        <source>process &quot;%1 %2&quot; crashed</source>
        <translation>prosessi &quot;%1 %2&quot; kaatui</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="249"/>
        <source>Failed to perform process &quot;%1 %2&quot;, error: %3</source>
        <translation>Prosessin &quot;%1 %2&quot; suorittaminen epäonnistui, virhe: %3</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="256"/>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="271"/>
        <source>&quot;%1&quot; is not a disk device</source>
        <translation>&quot;%1&quot; ei ole levylaite</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="298"/>
        <source>&quot;%1&quot; is busy</source>
        <translation>&quot;%1&quot; on kiireinen</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="325"/>
        <source>Failed to start &quot;%1 %2&quot;, error: %3</source>
        <translation>Tehtävän &quot;%1 %2&quot; suorittaminen epäonnistui, virhe: %3</translation>
    </message>
    <message>
        <location filename="../src/corelib/ddevicediskinfo.cpp" line="336"/>
        <source>Failed to open process, error: %1</source>
        <translation>Prosessia ei voitu avata, virhe: %1</translation>
    </message>
    <message>
        <location filename="../src/corelib/dfilediskinfo.cpp" line="178"/>
        <source>Failed to open file(%1), error: %2</source>
        <translation>Tiedoston (%1) avaaminen epäonnistui, virhe: %2</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="200"/>
        <source>%1 d %2 h %3 m</source>
        <translation>%1 päivää, %2 tuntia ja %3 minuuttia</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="203"/>
        <source>%1 h %2 m</source>
        <translation>%1 tuntia ja %2 minuuttia</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="206"/>
        <source>%1 m</source>
        <translation>%1 minuuttia</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="208"/>
        <source>%1 s</source>
        <translation>%1 sekuntia</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="152"/>
        <source>Deepin Clone</source>
        <translation>Deepinin Kloonaaja</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="153"/>
        <source>Deepin Clone is a tool to backup and restore in deepin. It supports to clone, backup and restore disk or partition and other functions.</source>
        <translation>Deepinin Kloonaaja on varmuuskopio- ja palautustyökalu. Se tukee levyjen ja osioiden kloonauksen, varmuuskopioinnin ja palautuksen lisäksi sun muita toiminnallisuuksia.</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="882"/>
        <source>Partition &quot;%1&quot; not found</source>
        <translation>Osiota &quot;%1&quot; ei löydetty</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="884"/>
        <source>Disk &quot;%1&quot; not found</source>
        <translation>Levyä &quot;%1&quot; ei löydetty</translation>
    </message>
    <message>
        <location filename="../src/corelib/helper.cpp" line="909"/>
        <location filename="../src/fixboot/bootdoctor.cpp" line="53"/>
        <location filename="../src/fixboot/bootdoctor.cpp" line="86"/>
        <location filename="../src/fixboot/bootdoctor.cpp" line="161"/>
        <source>Failed to mount partition &quot;%1&quot;</source>
        <translation>Osion &quot;%1&quot; liittäminen epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/fixboot/bootdoctor.cpp" line="173"/>
        <source>EFI partition not found</source>
        <translation>EFI-järjestelmäosiota ei löytynyt</translation>
    </message>
    <message>
        <location filename="../src/fixboot/bootdoctor.cpp" line="177"/>
        <source>Unknown partition table format</source>
        <translation>Tuntematon osiotaulukko</translation>
    </message>
    <message>
        <location filename="../src/fixboot/bootdoctor.cpp" line="199"/>
        <source>Boot for install system failed</source>
        <translation>Asennusjärjestelmään käynnistäminen epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/fixboot/bootdoctor.cpp" line="202"/>
        <source>Boot for update system failed</source>
        <translation>Päivitysjärjestelmään käynnistäminen epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/fixboot/bootdoctor.cpp" line="266"/>
        <source>Boot for repair system failed</source>
        <translation>Korjausjärjestelmään käynnistäminen epäonnistui</translation>
    </message>
</context>
<context>
    <name>SelectActionPage</name>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="75"/>
        <source>Select media</source>
        <translation>Valitse media</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="76"/>
        <source>Select operation for media</source>
        <translation>Valitse toiminto medialle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="85"/>
        <source>Disk</source>
        <translation>Levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="94"/>
        <source>Partition</source>
        <translation>Osio</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="119"/>
        <source>Clone Disk</source>
        <translation>Kloonaa Levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="119"/>
        <source>Clone source disk to target disk</source>
        <translation>Kloonaa lähdelevy kohdelevyyn</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="122"/>
        <source>Disk to Image</source>
        <translation>Levystä Kuvatiedostolle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="122"/>
        <source>Backup disk data to a image file</source>
        <translation>Varmuuskopioi levyn tiedot kuvatiedostoon</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="125"/>
        <source>Image to Disk</source>
        <translation>Kuvatiedostosta Levylle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="125"/>
        <source>Restore image file to disk</source>
        <translation>Palauta kuvatiedosto levylle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="131"/>
        <source>Clone Partition</source>
        <translation>Kloonaa Osio</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="131"/>
        <source>Clone source partition to target partition</source>
        <translation>Kloonaa lähdeosio kohdeosioon</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="134"/>
        <source>Partition to Image</source>
        <translation>Osiosta Kuvatiedostolle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="134"/>
        <source>Backup partition data to a image file</source>
        <translation>Varmuuskopioi osion tiedot kuvatiedostoon</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="137"/>
        <source>Image to Partition</source>
        <translation>Kuvatiedostosta Osiolle</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectactionpage.cpp" line="137"/>
        <source>Restore image file to partition</source>
        <translation>Palauta kuvatiedosto osioon</translation>
    </message>
</context>
<context>
    <name>SelectFilePage</name>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="322"/>
        <source>Select the source disk</source>
        <translation>Valitse lähdelevy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="323"/>
        <source>Select the target disk</source>
        <translation>Valitse kohdelevy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="337"/>
        <source>Select the source partition</source>
        <translation>Valitse lähdeosio</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="338"/>
        <source>Select the target partition</source>
        <translation>Valitse kohdeosio</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="366"/>
        <source>Select a disk to backup</source>
        <translation>Valitse varmuuskopioitava levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="374"/>
        <source>Select a partition to backup</source>
        <translation>Valitse varmuuskopioitava osio</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="383"/>
        <source>Select storage location</source>
        <translation>Valitse tietojen sijainti</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="409"/>
        <source>Select a backup image file</source>
        <translation>Valitse varmuuskopion kuvatiedosto</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="412"/>
        <source>Select a disk to restore</source>
        <translation>Valitse palautettava levy</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="414"/>
        <source>Select a partition to restore</source>
        <translation>Valitse palautettava osio</translation>
    </message>
</context>
<context>
    <name>SelectFileWidget</name>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="101"/>
        <source>Select storage location</source>
        <translation>Valitse tietojen sijainti</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="101"/>
        <source>Select image file</source>
        <translation>Valitse kuvatiedosto</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="107"/>
        <source>Drag and drop backup image file here</source>
        <translation>Raahaa ja pudota varmuuskopion kuvatiedosto tähän</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="223"/>
        <location filename="../src/widgets/selectfilepage.cpp" line="260"/>
        <source>Deepin Image File</source>
        <translation>Deepinin Kuvatiedosto</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="181"/>
        <source>Reselect image file</source>
        <translation>Valitse kuvatiedosto uudelleen</translation>
    </message>
    <message>
        <location filename="../src/widgets/selectfilepage.cpp" line="184"/>
        <source>Reselect storage location</source>
        <translation>Valitse tietojen sijainti uudelleen</translation>
    </message>
</context>
<context>
    <name>WorkingPage</name>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="44"/>
        <source>Task is ongoing, please wait......</source>
        <translation>Tehtävä on meneillään, ole hyvä ja odota...</translation>
    </message>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="61"/>
        <source>Progress: %1/%2</source>
        <translation>Edistyminen: %1/%2</translation>
    </message>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="62"/>
        <source>Remaining time: %1</source>
        <translation>Jäljellä oleva aika: %1</translation>
    </message>
    <message>
        <location filename="../src/widgets/workingpage.cpp" line="67"/>
        <source>Repairing system boot, please wait......</source>
        <translation>Korjataan järjestelmän käynnistystä, ole hyvä ja odota...</translation>
    </message>
</context>
</TS>