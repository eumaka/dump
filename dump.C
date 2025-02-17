
        SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

        TrkrClusterContainer *dst_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");

        ActsGeometry *_tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
        if(!_tgeometry)
        {
          std::cout << PHWHERE << "No Acts geometry on node tree. Can't  continue."
                  << std::endl;
        }

        for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
        {
            SvtxTrack* track = iter->second;
            px = track->get_px();
            py = track->get_py();
            pz = track->get_pz();

            mom.SetX(px); mom.SetY(py); mom.SetZ(pz);

            if(mom.Pt() < 1.0)
            {
                continue;
            }
            std::vector<int> _NClus = {0, 0};
            for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::inttId))
            {
                auto range = dst_clustermap->getClusters(hitsetkey);
                for (auto iter = range.first; iter != range.second; ++iter)
                {
                    TrkrDefs::cluskey ckey = iter->first;
                    TrkrCluster *cluster = dst_clustermap->findCluster(ckey);
                    unsigned int trkrId = TrkrDefs::getTrkrId(ckey);
                    if (trkrId != TrkrDefs::inttId)
                        continue; // we want only INTT clusters
                    int layer = (TrkrDefs::getLayer(ckey) == 3 || TrkrDefs::getLayer(ckey) == 4) ? 0 : 1;
                    _NClus[layer]++;
                    if (cluster == nullptr)
                    {
                        std::cout << "cluster is nullptr, ckey=" << ckey << std::endl;
                    }
                    auto globalpos = _tgeometry->getGlobalPosition(ckey, cluster);
                    ClusLayer_.push_back(TrkrDefs::getLayer(ckey));
                    TVector3 pos(globalpos(0), globalpos(1), globalpos(2));

                   ClusPhi_.push_back(pos.Phi());
                         }

            }

        }


                  
//    
